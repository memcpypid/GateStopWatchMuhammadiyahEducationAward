package main

import (
	"database/sql"
	"encoding/json"
	"log"
	"net/http"
	"os"
	"strings"
	"sync"
	"time"

	"github.com/gin-contrib/cors"
	"github.com/gin-gonic/gin"
	"github.com/gorilla/websocket"
	_ "github.com/mattn/go-sqlite3"
)

type TimerState struct {
	Running            bool   `json:"running"`
	ElapsedMs          int64  `json:"elapsed_ms"`
	Status             string `json:"status"`
	StartTime          int64  `json:"start_time_unix_ms"`
	FinishTime         int64  `json:"finish_time_unix_ms"`
	StartTimeStopWatch int64  `json:"start_time_stopwatch_unix_ms"`
	StopwatchRunning   bool   `json:"stopwatch_running"`
	StopwatchElapsed   int64  `json:"stopwatch_elapsed_ms"`
	StopwatchMaxMs     int64  `json:"stopwatch_max_ms"`
}

type HistoryEntry struct {
	ID        int64 `json:"id"`
	ElapsedMs int64 `json:"elapsed_ms"`
	Timestamp int64 `json:"timestamp"`
}

type Client struct {
	conn      *websocket.Conn
	send      chan []byte
	closeOnce sync.Once
	closed    chan struct{}
}

var (
	state = &TimerState{
		Status:         "Idle",
		StopwatchMaxMs: 10 * 60 * 1000,
	}
	stateMu sync.RWMutex

	wsClients = make(map[*Client]bool)
	wsMutex   sync.Mutex

	upgrader = websocket.Upgrader{CheckOrigin: func(r *http.Request) bool {
		return true
	}}

	db      *sql.DB
	dbMutex sync.Mutex
)

const (
	writeWait      = 10 * time.Second
	pongWait       = 60 * time.Second
	pingPeriod     = pongWait * 9 / 10
	maxMessageSize = 1024
)

func initDB() error {
	var err error
	db, err = sql.Open("sqlite3", "./timer.db")
	if err != nil {
		return err
	}

	// WAL mode sangat penting untuk Raspberry Pi
	_, _ = db.Exec(`PRAGMA journal_mode=WAL;`)
	_, _ = db.Exec(`PRAGMA synchronous=NORMAL;`)

	createTableSQL := `
	CREATE TABLE IF NOT EXISTS history (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		elapsed_ms INTEGER NOT NULL,
		timestamp INTEGER NOT NULL
	);
	`

	_, err = db.Exec(createTableSQL)
	if err != nil {
		return err
	}

	return nil
}

func saveToHistory(elapsedMs int64) error {
	dbMutex.Lock()
	defer dbMutex.Unlock()

	_, err := db.Exec(`INSERT INTO history(elapsed_ms, timestamp) VALUES (?, ?)`, elapsedMs, time.Now().UnixMilli())
	return err
}

func getHistory() ([]HistoryEntry, error) {
	dbMutex.Lock()
	defer dbMutex.Unlock()

	rows, err := db.Query(`SELECT id, elapsed_ms, timestamp FROM history ORDER BY id ASC`)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	list := make([]HistoryEntry, 0, 100)
	for rows.Next() {
		var e HistoryEntry
		if err := rows.Scan(&e.ID, &e.ElapsedMs, &e.Timestamp); err != nil {
			return nil, err
		}
		list = append(list, e)
	}
	return list, nil
}

func clearHistory() error {
	dbMutex.Lock()
	defer dbMutex.Unlock()

	_, err := db.Exec(`DELETE FROM history`)
	return err
}

func main() {
	if err := initDB(); err != nil {
		log.Fatal(err)
	}
	defer db.Close()

	router := gin.Default()

	router.Use(cors.New(cors.Config{
		AllowOrigins: []string{"*"},
		AllowMethods: []string{"GET", "POST", "OPTIONS"},
		AllowHeaders: []string{"Content-Type"},
	}))

	router.GET("/health", func(c *gin.Context) { c.JSON(200, gin.H{"status": "ok"}) })
	router.GET("/status", statusHandler)
	router.POST("/trigger", triggerHandler)
	router.GET("/ws", wsHandler)
	router.GET("/history", historyHandler)
	router.POST("/history/clear", clearHistoryHandler)

	go timerTicker()

	port := os.Getenv("PORT")
	if port == "" {
		port = "3000"
	}
	log.Println("Running on port:", port)

	router.Run(":" + port)
}

func statusHandler(c *gin.Context) {
	stateMu.RLock()
	s := *state
	stateMu.RUnlock()
	c.JSON(200, s)
}

func historyHandler(c *gin.Context) {
	data, err := getHistory()
	if err != nil {
		c.JSON(500, gin.H{"error": err.Error()})
		return
	}
	c.JSON(200, gin.H{"success": true, "data": data})
}

func clearHistoryHandler(c *gin.Context) {
	if err := clearHistory(); err != nil {
		c.JSON(500, gin.H{"error": err.Error()})
		return
	}
	c.JSON(200, gin.H{"success": true})
}

func triggerHandler(c *gin.Context) {
	raw, _ := c.GetRawData()
	trig := strings.TrimSpace(string(raw))

	if strings.HasPrefix(trig, "{") {
		var p map[string]string
		_ = json.Unmarshal(raw, &p)
		if v, ok := p["trigger"]; ok {
			trig = v
		}
	}

	handleTrigger(trig)
	c.JSON(200, gin.H{"success": true, "trigger": trig})
}

func wsHandler(c *gin.Context) {
	conn, err := upgrader.Upgrade(c.Writer, c.Request, nil)
	if err != nil {
		log.Println("WS upgrade failed:", err)
		return
	}

	client := &Client{
		conn:   conn,
		send:   make(chan []byte, 256),
		closed: make(chan struct{}),
	}

	wsMutex.Lock()
	wsClients[client] = true
	wsMutex.Unlock()

	go client.writePump()
	go client.readPump()
	client.sendSnapshot()
}

func (c *Client) readPump() {
	defer c.close()

	c.conn.SetReadLimit(maxMessageSize)
	c.conn.SetReadDeadline(time.Now().Add(pongWait))
	c.conn.SetPongHandler(func(string) error {
		c.conn.SetReadDeadline(time.Now().Add(pongWait))
		return nil
	})

	for {
		var msg map[string]string
		if err := c.conn.ReadJSON(&msg); err != nil {
			break
		}
		if t, ok := msg["trigger"]; ok {
			handleTrigger(t)
		}
	}
}

func (c *Client) writePump() {
	ticker := time.NewTicker(pingPeriod)
	defer func() {
		ticker.Stop()
		c.close()
	}()

	for {
		select {
		case m, ok := <-c.send:
			c.conn.SetWriteDeadline(time.Now().Add(writeWait))
			if !ok {
				c.conn.WriteMessage(websocket.CloseMessage, []byte{})
				return
			}
			if err := c.conn.WriteMessage(websocket.TextMessage, m); err != nil {
				return
			}

		case <-ticker.C:
			c.conn.SetWriteDeadline(time.Now().Add(writeWait))
			if err := c.conn.WriteMessage(websocket.PingMessage, nil); err != nil {
				return
			}

		case <-c.closed:
			return
		}
	}
}

func (c *Client) sendSnapshot() {
	stateMu.RLock()
	b, err := json.Marshal(state)
	stateMu.RUnlock()
	if err != nil {
		return
	}
	select {
	case c.send <- b:
	default:
	}
}

func (c *Client) close() {
	c.closeOnce.Do(func() {
		wsMutex.Lock()
		delete(wsClients, c)
		wsMutex.Unlock()

		close(c.send)
		c.conn.Close()
		close(c.closed)
	})
}

func handleTrigger(t string) {
	t = strings.ToLower(strings.TrimSpace(t))
	now := time.Now().UnixMilli()

	var toSave int64 = 0

	stateMu.Lock()
	switch t {
	case "start":
		if !state.Running {
			state.Running = true
			state.StartTime = now
			state.ElapsedMs = 0
			state.Status = "Running"
			// Stopwatch starts with Timer Lapse
			if !state.StopwatchRunning {
				state.StopwatchRunning = true
				state.StopwatchElapsed = 0
				state.StartTimeStopWatch = now
			}

		}

	case "finish":
		if state.Running {
			state.Running = false
			state.FinishTime = now
			state.ElapsedMs = now - state.StartTime
			state.Status = "Finished"
			toSave = state.ElapsedMs
		}
		// Stopwatch continues running even after Timer Lapse finishes

	case "reset":
		state.Running = false
		state.Status = "Idle"
		state.ElapsedMs = 0
		state.StartTime = 0
		state.FinishTime = 0
		// Reset stopwatch only when explicitly requested
		state.StopwatchRunning = false
		state.StopwatchElapsed = 0
		state.StartTimeStopWatch = 0
	}
	stateMu.Unlock()

	if toSave > 0 {
		go saveToHistory(toSave)
	}

	broadcastStatus()
}

func timerTicker() {
	ticker := time.NewTicker(20 * time.Millisecond)
	defer ticker.Stop()

	for range ticker.C {

		stateMu.Lock()
		if state.Running {
			state.ElapsedMs = time.Now().UnixMilli() - state.StartTime
		}

		// Handle stopwatch countdown
		if state.StopwatchRunning {
			state.StopwatchElapsed = time.Now().UnixMilli() - state.StartTimeStopWatch

			// Stop stopwatch when 10 minutes have passed
			if state.StopwatchElapsed >= state.StopwatchMaxMs {
				state.StopwatchRunning = false
				state.StopwatchElapsed = 0
			}
		}

		running := state.Running
		stopwatchRunning := state.StopwatchRunning
		stateMu.Unlock()

		if running || stopwatchRunning {
			broadcastStatus()
		}
	}
}

func broadcastStatus() {
	stateMu.RLock()
	b, err := json.Marshal(state)
	stateMu.RUnlock()
	if err != nil {
		return
	}

	wsMutex.Lock()
	for c := range wsClients {
		select {
		case c.send <- b:
		default:
			go c.close()
		}
	}
	wsMutex.Unlock()
}
