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
    let socket = SocketIOClient(socketURL: "128.220.247.211:40888")

    // Left card in View.
    @IBOutlet weak var leftCard: UIImageView!
    
    // Right card in View.
    @IBOutlet weak var rghtCard: UIImageView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.

        // Add all socket handlers here.
        self.addHandlers()

        // Send the string RCARD to the dumb server.
        self.socket.connect()
    }

    // Add handlers for any type of socket event.
    func addHandlers() {
        // Handler called on any socket event.
        self.socket.onAny {println(
            "Got event: \($0.event), with items: \($0.items)")
        }
        self.socket.on("connection"
    }

    // Take the server response and parse it to set the card image.
    func parseServerResponse() {

        // Set the left card image.
        self.leftCard.image = UIImage(named: "Ad")
        
        // Set the right card image.
        self.rghtCard.image = UIImage(named: "Ad")
    }
    
    
    @IBAction func getCardsPressed(sender: AnyObject) {
        //let data = "RCARD".dataUsingEncoding(NSUTF8StringEncoding, allowLossyConversion: false)!
        //socket.emit("GetCards", data)
        
        // Send the RCARD message to
        socket.emit("GetCards", "RCARD")
    }
    
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

