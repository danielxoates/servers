from http.server import BaseHTTPRequestHandler, HTTPServer
import time

hostName = 'localhost'
port = 80

class myServer(BaseHTTPRequestHandler):
    def do_GET(self):
        file = open('../index.html', 'r')
        contents = bytes(file.read(), "utf-8")
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers
        self.wfile.write(contents)

if __name__ == "__main__":
    webServer = HTTPServer((hostName, port), myServer)
    print("server started at:", hostName, port)
    
    try:
        webServer.serve_forever()
    except KeyboardInterrupt:
        pass
    
    webServer.server_close()
    print("Server stopped")