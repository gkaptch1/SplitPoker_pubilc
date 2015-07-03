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
    let sock = TCPIPSocket()
    
    // Something for this variable.
    var w = false
    
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
    func parseServerResponse(srvRsp: String) {

        // Parse the server response string into an array.
        var srvRspArray = split(srvRsp) {$0 == " "}
        var device: String = srvRspArray[0]
        var table: String = srvRspArray[1] + srvRspArray[2]
        var hole_cards: Int = srvRspArray[3].toInt()!
        var strLCard: String = srvRspArray[4]
        var strRCard: String = srvRspArray[5]
        
        // Set the left card image.
        self.leftCard.image = UIImage(named: strLCard)
        
        // Set the right card image.
        self.rghtCard.image = UIImage(named: strRCard)
    }
    
    // UI button press.
    @IBAction func getCardsPressed(sender: AnyObject) {
        // Send the RCARD message to poker server.
        // Use NSFileHandle for writing data.
        let fHandle = NSFileHandle(fileDescriptor: self.sock.socketDescriptor)
        
        // Synchronous.
        //fHandle.writeData(("RCARD" as NSString).dataUsingEncoding(NSUTF8StringEncoding)!)
        //let dataFromServer = fHandle.readDataToEndOfFile()
        //println(NSString(data: dataFromServer, encoding: NSUTF8StringEncoding)!)
        
        
        // Asyncrhonous.
        fHandle.writeabilityHandler = { (f: NSFileHandle!) -> () in
            if self.w == false {
                self.w =  true
                fHandle.writeData(("RCARD\n" as NSString).dataUsingEncoding(NSUTF8StringEncoding)!)
            }
        }
        
        fHandle.readabilityHandler = { (f: NSFileHandle!) -> () in
            let srvRsp = NSString(data: fHandle.availableData, encoding: NSUTF8StringEncoding)!
            println("Server response: \(srvRsp)")
            self.parseServerResponse(srvRsp as String)
        }
    
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

