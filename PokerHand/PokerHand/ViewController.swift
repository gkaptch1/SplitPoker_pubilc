//
//  ViewController.swift
//  PokerHand
//
//  Created by Michael Rushanan on 7/2/15.
//  Copyright (c) 2015 Michael Rushanan. All rights reserved.
//

import UIKit
import Foundation


// Our single window View Controller.
class ViewController: UIViewController {

    // Create a socket to virtual machine on port 40888.
    //let socket = SocketIOClient(socketURL: "128.220.247.211:40888")
    let sock = TCPIPSocket()
    
    // Left card in View.
    @IBOutlet weak var leftCard: UIImageView!
    
    // Right card in View.
    @IBOutlet weak var rghtCard: UIImageView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.

        // TCP connection to the poker server.
        self.sock.connect(TCPIPSocketAddress(128, 220, 247, 211), 40888)
    }

    // Take the server response and parse it to set the card image.
    func parseServerResponse() {

        // Set the left card image.
        self.leftCard.image = UIImage(named: "Ad")
        
        // Set the right card image.
        self.rghtCard.image = UIImage(named: "Ad")
    }
    
    // UI button press.
    @IBAction func getCardsPressed(sender: AnyObject) {
        // Send the RCARD message to poker server.
        // Use NSFileHandle for writing data.
        let fHandle = NSFileHandle(fileDescriptor: self.sock.socketDescriptor)
        
        fHandle.writeData(("RCARD" as NSString).dataUsingEncoding(NSUTF8StringEncoding)!)
        let dataFromServer = fHandle.readDataToEndOfFile()
        
        println(NSString(data: dataFromServer, encoding: NSUTF8StringEncoding)!)
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

