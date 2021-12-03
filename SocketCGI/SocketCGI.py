from http.server import HTTPServer, CGIHTTPRequestHandler
port = 8080
server_address = ("", port)
print('port', port)
httpd = HTTPServer(server_address, CGIHTTPRequestHandler)
httpd.serve_forever()
