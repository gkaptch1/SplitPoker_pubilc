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
    let client:TCPClient = TCPClient(addr: "128.220.247.211", port: 40888)

    // Left card in View.
    @IBOutlet weak var leftCard: UIImageView!
    
    // Right card in View.
    @IBOutlet weak var rghtCard: UIImageView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.

        // TCP connection to the poker server.
        var(success, errmsg) = self.client.connect(timeout: 10)
        
        if !success {
            println(errmsg)
        }
    }
    
    // UI button press.
    @IBAction func getCardsPressed(sender: AnyObject) {
        // Send the RCARD message to poker server.
        // Use NSFileHandle for writing data.
        var(success, errmsg) = self.client.send(str:"RCARD\n")
        
        if success {
            // Get response from poker server.
            var data = client.read(1024*10)
            if let d = data {
                var srvResp = String(bytes: d, encoding: NSUTF8StringEncoding)
                println("Server response from Get Cards: \(srvResp)")
                parseServerResponse(srvResp!)
            }
        } else {
            println(errmsg)
        }
    }
    
    @IBAction func getRegisterDevice(sender: AnyObject) {
        //Send a message to the server that inc
        // PROTOCOL: PDEVICE <VERSION> <UUID>
        var(success, errmsg) = self.client.send(str:"PDEVICE 5 f154c24f-4c72-4ad2-a6a3-3ee015666cfc\n")
        
        if success {
            // Get response from poker server.
            var data = client.read(1024*10)
            if let d = data {
                var srvResp = String(bytes: d, encoding: NSUTF8StringEncoding)
                println("Server response from Register: \(srvResp)")
            }
        } else {
            println(errmsg);
        }
        
    }

    // Take the server response and parse it to set the card image.
    func parseServerResponse(srvRsp: String) {
        
        // Parse the server response string into an array.
        let srvRsp = srvRsp.stringByReplacingOccurrencesOfString("\r\n", withString: "")
        var srvRspArray = split(srvRsp) {$0 == " "}
        var device: String = srvRspArray[0]
        var table: String = srvRspArray[1] + srvRspArray[2]
        var hole_cards: Int = srvRspArray[3].toInt()!
        var strLCard: String = srvRspArray[4]
        var strRCard: String = srvRspArray[5]
        
        //println("\(device) \(table) \(hole_cards) \(strLCard) \(strRCard)")
        
        // Set the left card image.
        self.leftCard.image = UIImage(named: strLCard)
        
        // Set the right card image.
        self.rghtCard.image = UIImage(named: strRCard)
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

