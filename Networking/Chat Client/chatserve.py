#!/usr/bin/python

'''
Name: Aaron Boutin      Assignment: Project 1           Date: 11/FEB/2017

References/Sources: 
    CS372 Lectures and Python Software Foundation (PSF) for general setup. Stackoverflow and PSF
    for syntax/semantic questions.

Extra Credit: I have the progam use threading to implement concurrent send/recieve between client and host
'''

import SocketServer
import sys
from threading import Thread

'''
STARTUP

Function initiates verification between server and client, takes self as argument
'''
def startUp(self):
        # Begin Verification
        sys.stdin.flush()
        self.data = self.request.recv(1024).strip()
        if self.data != "PORTNUM":
            print "Incompatible Connection"
            return
        sys.stdout.flush()
        self.request.send("PORTNUM\0")
        # End Verification        

'''
RECVLOOP
Function is designed for use in thread for concurrent input/output. Takes self and client's handle as argument
'''
def recvLoop(self, clientHandle):
    while(self.data != "\quit"):
        self.data = self.request.recv(1024).strip()
        print clientHandle + "> " + self.data
        if self.data == "\quit":
            global chatDone
            chatDone = 1
        if chatDone == 1:
            print "Client has ended the conversation, please enter \quit to close program"
            break    

'''
SENDLOOP
Function used for output to client while thread handles reciept of client messages. Takes self and server's handle as arguments.
'''
def sendLoop(self, serverHandle):

    output = ""
    text = ""
    while(text != "\quit"):
        sys.stdout.flush()
        text = raw_input() 
        if text == "\quit":
            global chatDone
            chatDone = 1 
        output = text
        self.request.send(output)

'''
class MyTCPHandler
handle function is overridden with server functionality upon connection to client.
'''  
class MyTCPHandler(SocketServer.BaseRequestHandler):

    def handle(self):

        startUp(self)                                            #Start Function that verifies connection

        # Get and Recieve handles
        print "Welcome to A-CHAT"
        name = raw_input("Please Enter a name for the Handle you would like to use: ")
        self.request.send(name)
        print "Please wait while the Client's Handle is acquired."
        self.data = self.request.recv(1024).strip()
        clientHandle = self.data
        print "You may begin chatting"
        sys.stdin.flush()        
        sys.stdout.flush()

        # Start Conversation
        t = Thread(target=recvLoop, args=(self,clientHandle,))   # Thread for Receving function
        t.start()

        sendLoop(self, name)                                     # Sending Function loop      
        t.join()                                                 # Join thread when complete
        
        global chatDone
        chatDone = 0 
        print "Session Ended, awaiting new connection."        

if __name__ == "__main__":
    HOST, PORT = "localhost", int(sys.argv[1])
    chatDone = 0                                                # Global variable used to help send and recieve function know when either is terminating.
    server = SocketServer.TCPServer((HOST, PORT), MyTCPHandler)
    server.serve_forever()                                      # Run until signal recieved