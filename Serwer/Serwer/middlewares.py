# import socketserver
# import threading
# import atexit

# class MyThread(threading.Thread):
#   def run(self):
#     HOST, PORT = "localhost", 9999

#     # Create the server, binding to localhost on port 9999
#     SocketMiddleware.server = socketserver.TCPServer((HOST, PORT), MySocket)

#     # Activate the server; this will keep running until you
#     # interrupt the program with Ctrl-C
#     SocketMiddleware.server.serve_forever()

class SocketMiddleware(object):
  server = None
  statushash = {}

  # def close(self):
  #   print("Closing socket, wreszcie")
  #   server.shutdown()

  # def __init__(self):
  #   atexit.register(self.close)
  #   thread = MyThread()
  #   thread.start()

# class MySocket(socketserver.BaseRequestHandler):
#     """
#     The RequestHandler class for our server.

#     It is instantiated once per connection to the server, and must
#     override the handle() method to implement communication to the
#     client.
#     """

#     def handle(self):
#         # self.request is the TCP socket connected to the client
#         self.data = self.request.recv(1024).strip()
#         info = self.data.decode("UTF-8").split(":")
#         task_id = int(info[0])
#         percent = int(info[1])
#         done = int(info[2])
#         SocketMiddleware.statushash[task_id] = (percent, done)
        
#         print("{} wrote:".format(self.client_address[0]))
#         print(self.data)
        # just send back the same data, but upper-cased
        #self.request.sendall(self.data.upper())

#if __name__ == "__main__":
#    HOST, PORT = "localhost", 9999

    # Create the server, binding to localhost on port 9999
#    server = SocketServer.TCPServer((HOST, PORT), MyTCPHandler)

    # Activate the server; this will keep running until you
    # interrupt the program with Ctrl-C
    #server.serve_forever()
