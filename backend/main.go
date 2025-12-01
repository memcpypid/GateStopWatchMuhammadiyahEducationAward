package main

import (
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
)

// TimerState menyimpan status timer. Akses harus dikunci dengan stateMu.
type TimerState struct {
	Running    bool   `json:"running"`
	ElapsedMs  int64  `json:"elapsed_ms"`
	Status     string `json:"status"`
	StartTime  int64  `json:"start_time_unix_ms"`
	FinishTime int64  `json:"finish_time_unix_ms"`
}

// Client mewakili koneksi websocket per-klien dengan pump terpisah
type Client struct {
	conn      *websocket.Conn
	send      chan []byte
	closeOnce sync.Once
	closed    chan struct{}
}

var (
	state     = &TimerState{Status: "Idle"}
	stateMu   = sync.Mutex{}           // protects access to state
	wsClients = make(map[*Client]bool) // active clients
	wsMutex   = sync.Mutex{}           // protects wsClients
	upgrader  = websocket.Upgrader{CheckOrigin: func(r *http.Request) bool { return true }}
)

// WebSocket control constants
const (
	writeWait      = 10 * time.Second
	pongWait       = 60 * time.Second
	pingPeriod     = (pongWait * 9) / 10
	maxMessageSize = 512
)

func main() {
	// Use Gin
	router := gin.Default()

	// CORS - allow any origin (adjust for production as needed)
	router.Use(cors.New(cors.Config{
		AllowOrigins:     []string{"*"},
		AllowMethods:     []string{"GET", "POST", "OPTIONS"},
		AllowHeaders:     []string{"Content-Type"},
		ExposeHeaders:    []string{"Content-Length"},
		AllowCredentials: true,
		MaxAge:           12 * time.Hour,
	}))

	// Routes
	router.GET("/health", healthHandler)    // quick health check
	router.GET("/status", statusHandler)    // HTTP status snapshot
	router.POST("/trigger", triggerHandler) // trigger via HTTP (plain text or JSON)
	router.GET("/ws", wsHandler)            // websocket endpoint

	// Start ticker (updates + broadcast)
	go timerTicker()

	port := os.Getenv("PORT")
	if port == "" {
		port = "3000"
	}

	log.Println("🚀 Backend Timer running on port", port)
	if err := router.Run(":" + port); err != nil {
		log.Fatalf("Server failed: %v", err)
	}
}

func healthHandler(c *gin.Context) {
	c.JSON(200, gin.H{"status": "ok"})
}

func statusHandler(c *gin.Context) {
	stateMu.Lock()
	copyState := *state
	stateMu.Unlock()
	c.JSON(200, copyState)
}

func triggerHandler(c *gin.Context) {
	bodyBytes, err := c.GetRawData()
	if err != nil {
		c.JSON(400, gin.H{"error": "Failed to read body"})
		return
	}

	trigger := strings.TrimSpace(string(bodyBytes))
	// Accept JSON {"trigger":"start"} as well
	if strings.HasPrefix(trigger, "{") {
		var payload map[string]string
		if err := json.Unmarshal(bodyBytes, &payload); err == nil {
			if t, ok := payload["trigger"]; ok {
				trigger = t
			}
		}
	}

	log.Printf("📥 HTTP trigger received: %s", trigger)
	handleTrigger(trigger)

	c.JSON(200, gin.H{
		"success":   true,
		"message":   "Trigger received",
		"trigger":   trigger,
		"timestamp": time.Now().Format(time.RFC3339Nano),
	})
}

func wsHandler(c *gin.Context) {
	conn, err := upgrader.Upgrade(c.Writer, c.Request, nil)
	if err != nil {
		log.Println("❌ WebSocket upgrade error:", err)
		return
	}

	client := &Client{
		conn:   conn,
		send:   make(chan []byte, 256),
		closed: make(chan struct{}),
	}

	// register client
	wsMutex.Lock()
	wsClients[client] = true
	wsMutex.Unlock()

	log.Println("📡 WebSocket client connected. Total clients:", clientCount())

	// start pumps
	go client.writePump()
	go client.readPump()

	// send immediate snapshot
	client.sendSnapshot()
}

// clientCount returns number of connected clients (with mutex)
func clientCount() int {
	wsMutex.Lock()
	n := len(wsClients)
	wsMutex.Unlock()
	return n
}

// readPump reads messages from the websocket connection and handles triggers.
// It sets read limits and pong handler for keepalive.
func (c *Client) readPump() {
	defer c.close()

	c.conn.SetReadLimit(maxMessageSize)
	_ = c.conn.SetReadDeadline(time.Now().Add(pongWait))
	c.conn.SetPongHandler(func(string) error {
		_ = c.conn.SetReadDeadline(time.Now().Add(pongWait))
		return nil
	})

	for {
		var msg map[string]string
		if err := c.conn.ReadJSON(&msg); err != nil {
			// when client disconnects or sends invalid JSON, exit
			if websocket.IsUnexpectedCloseError(err, websocket.CloseGoingAway, websocket.CloseAbnormalClosure) {
				log.Println("❌ WebSocket read error:", err)
			}
			break
		}
		// handle trigger if present
		if trigger, ok := msg["trigger"]; ok {
			log.Printf("📥 WS trigger received: %s", trigger)
			handleTrigger(trigger)
		}
	}
}

// writePump writes messages from send channel to websocket connection and sends periodic pings.
func (c *Client) writePump() {
	ticker := time.NewTicker(pingPeriod)
	defer func() {
		ticker.Stop()
		c.close()
	}()

	for {
		select {
		case message, ok := <-c.send:
			_ = c.conn.SetWriteDeadline(time.Now().Add(writeWait))
			if !ok {
				// send channel closed
				_ = c.conn.WriteMessage(websocket.CloseMessage, []byte{})
				return
			}
			if err := c.conn.WriteMessage(websocket.TextMessage, message); err != nil {
				return
			}

		case <-ticker.C:
			_ = c.conn.SetWriteDeadline(time.Now().Add(writeWait))
			if err := c.conn.WriteMessage(websocket.PingMessage, nil); err != nil {
				return
			}
		case <-c.closed:
			return
		}
	}
}

// sendSnapshot sends current state immediately to this client (non-blocking)
func (c *Client) sendSnapshot() {
	stateMu.Lock()
	data, err := json.Marshal(state)
	stateMu.Unlock()
	if err != nil {
		return
	}
	select {
	case c.send <- data:
	default:
		// if channel full, drop
	}
}

// close removes client from registry and closes underlying resources once.
func (c *Client) close() {
	c.closeOnce.Do(func() {
		// unregister
		wsMutex.Lock()
		if _, ok := wsClients[c]; ok {
			delete(wsClients, c)
		}
		wsMutex.Unlock()

		// close send channel to signal writer to finish
		close(c.send)

		// close websocket connection
		_ = c.conn.Close()

		// signal closed
		close(c.closed)

		log.Println("📡 WebSocket client disconnected. Total clients:", clientCount())
	})
}

func handleTrigger(trigger string) {
	trigger = strings.ToLower(strings.TrimSpace(trigger))
	now := time.Now().UnixMilli()
	log.Println("Received trigger:", trigger)

	stateMu.Lock()
	switch trigger {
	case "start":
		if !state.Running {
			state.Running = true
			state.StartTime = now
			state.Status = "Running"
			state.FinishTime = 0
			state.ElapsedMs = 0
			log.Println("⏱️  Timer started")
		}
	case "finish":
		if state.Running {
			state.Running = false
			state.FinishTime = now
			state.ElapsedMs = state.FinishTime - state.StartTime
			state.Status = "Finished"
			log.Println("⏹️  Timer finished. Elapsed:", state.ElapsedMs, "ms")
		}
	case "reset":
		state.Running = false
		state.ElapsedMs = 0
		state.StartTime = 0
		state.FinishTime = 0
		state.Status = "Idle"
		log.Println("🔄 Timer reset")
	default:
		log.Println("⚠️  Unknown trigger:", trigger)
	}
	stateMu.Unlock()

	// broadcast immediately after state change (unlock first to avoid deadlock)
	broadcastStatus()
}

func timerTicker() {
	log.Println("⏱️  Timer ticker started")
	ticker := time.NewTicker(10 * time.Millisecond) // 20 Hz updates
	defer ticker.Stop()

	tickCount := 0
	for range ticker.C {
		tickCount++

		stateMu.Lock()
		if state.Running {
			now := time.Now().UnixMilli()
			state.ElapsedMs = now - state.StartTime
		}
		isRunning := state.Running
		elapsed := state.ElapsedMs
		stateMu.Unlock()
		if isRunning {
			// broadcast regularly
			broadcastStatus()
		}
		// periodic log every ~1 second
		if tickCount%20 == 0 {
			log.Printf("⏱️  Tick #%d - Elapsed: %dms, Running: %v, Clients: %d\n", tickCount, elapsed, isRunning, clientCount())
		}
	}
}

// broadcastStatus marshals the state and pushes to all clients' send channel (non-blocking).
func broadcastStatus() {
	stateMu.Lock()
	data, err := json.Marshal(state)
	stateSnapshot := *state
	stateMu.Unlock()

	if err != nil {
		log.Println("❌ Error marshaling state:", err)
		return
	}

	wsMutex.Lock()
	clientCount := len(wsClients)
	defer wsMutex.Unlock()

	log.Printf("📤 Broadcasting to %d clients - Running: %v, Elapsed: %dms\n", clientCount, stateSnapshot.Running, stateSnapshot.ElapsedMs)

	for client := range wsClients {
		select {
		case client.send <- data:
			// queued
		default:
			// client send channel full -> disconnect it to recover
			log.Println("⚠️ Client send buffer full — disconnecting client to recover")
			go client.close()
		}
	}
}
