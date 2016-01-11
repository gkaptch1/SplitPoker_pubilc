//
//  ViewController.swift
//  PokerHand
//

import UIKit
import Foundation


// Our single window View Controller.
class ViewController: UIViewController {

    // Create a socket to virtual machine on port 40888.
    var client:TCPClient = TCPClient(addr: "game.secureonlinepoker.com", port: 40888)

    //Text Field representing server IP
    @IBOutlet weak var serverIP: UITextField!
    
    //Text Field representing the client's unique user ID
    @IBOutlet weak var clientUuid: UITextField!
    
    //Var representing the software version.  Dummy at this point
    var version:String = "5"
    
    // Left card in View.
    @IBOutlet weak var leftCard: UIImageView!
    
    // Right card in View.
    @IBOutlet weak var rghtCard: UIImageView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.

        // TCP connection to the poker server.
        clientUuid.text = ""
        serverIP.text = "game.secureonlinepoker.com"
    }
    
    // UI button press.
    @IBAction func getCardsPressed(sender: AnyObject) {
        // Send the RCARD message to poker server.
        // Use NSFileHandle for writing data.
        var(success, errmsg) = self.client.send(str:"RCARD\n")
        print(errmsg)
        if !success && errmsg ==  "socket not open" {
            if (self.serverIP.text != "") {
                self.client.close()
                self.client = TCPClient(addr: serverIP.text!, port: 40888)
                self.client.connect(timeout: 10)
            } else {
                self.client.close()
                self.client.connect(timeout: 10)
            }
            (success, errmsg) = self.client.send(str:"RCARD\n")
        }
        
        if success {
            // Get response from poker server.
            let data = client.read(1024*10)
            if let d = data {
                let srvResp = String(bytes: d, encoding: NSUTF8StringEncoding)
                print("Server response from Get Cards: \(srvResp)")
                let cards = parseServerResponse(srvResp!)
                updateCardImages(cards.lCard, strRCard: cards.rCard)
            }
        } else {
            let alert = UIAlertController(title: "Network Problem", message: "You are unable to connect to the server.  Try a different server", preferredStyle: UIAlertControllerStyle.Alert)
            let alertAction = UIAlertAction(title: "OK", style: UIAlertActionStyle.Default) { (UIAlertAction) -> Void in }
            alert.addAction(alertAction)
            presentViewController(alert, animated: true) { () -> Void in }
            return

        }
    }
    
    @IBAction func getRegisterDevice(sender: AnyObject) {
        // Update the tcp connection to make sure it is pointing at the desired server ( if the ip field is not empty
        if (self.serverIP.text != "") {
            self.client.close()
            self.client = TCPClient(addr: serverIP.text!, port: 40888)
            self.client.connect(timeout: 10)
        } else {
            self.client.close()
            self.client.connect(timeout: 10)
        }
        
        
        
        //Send a message to the server that inc
        // PROTOCOL: PDEVICE <VERSION> <UUID>
        // Check if the uuid field is empty
        var registrationMessage = ""
        if ( self.clientUuid.text == "" ) {
            registrationMessage = "PDEVICE \(version) f154c24f-4c72-4ad2-a6a3-3ee015666cfc\n"
        } else {
            registrationMessage = "PDEVICE \(version) \(self.clientUuid.text!)\n"
        }
        
        print(registrationMessage, terminator: "")
        
        var(success, errmsg) = self.client.send(str:registrationMessage)
        
        if success {
            // Get response from poker server.
            var data = client.read(1024*10)
            if let d = data {
                let srvResp = String(bytes: d, encoding: NSUTF8StringEncoding)
                let isUUIDErrorMsg = srvResp!.containsString("ERR 32")
                if ( isUUIDErrorMsg ) {
                    print(srvResp)
                    let alert = UIAlertController(title: "Invalid UUID", message: "Please check that your computer client is connected your device has a matching UUID", preferredStyle: UIAlertControllerStyle.Alert)
                    let alertAction = UIAlertAction(title: "OK", style: UIAlertActionStyle.Default) { (UIAlertAction) -> Void in }
                    alert.addAction(alertAction)
                    presentViewController(alert, animated: true) { () -> Void in }
                    return
                }
                print("Server response from Register: \(srvResp)")
            }
            
            //Listen async for incoming messages from the server updating the cards displayed
            let priority = DISPATCH_QUEUE_PRIORITY_DEFAULT
            dispatch_async(dispatch_get_global_queue(priority, 0)) {
                // Listen forever
                while true {
                    data = self.client.read(1024*10)
                    if let d = data {
                        let srvResp = String(bytes: d, encoding: NSUTF8StringEncoding)
                        print("Server response from Continual Listen: \(srvResp)")
                        if (srvResp != nil) {
                            let cards = self.parseServerResponse(srvResp!)
                            //Do the UI update on the main thread
                            dispatch_async(dispatch_get_main_queue()) {
                                print("Updating Cards on Main Thread")
                                self.updateCardImages(cards.lCard, strRCard: cards.rCard)
                            }
                        }
                    }
                }
            }
            
        } else {
            print(errmsg);
        }
        
    }

    // Take the server response and parse it to set the card image.
    func parseServerResponse(srvRsp: String) -> (lCard: String, rCard: String) {
        
        // Parse the server response string into an array.
        let srvRsp = srvRsp.stringByReplacingOccurrencesOfString("\r\n", withString: "")
        var srvRspArray = srvRsp.characters.split {$0 == " "}.map { String($0) }
        var device: String = srvRspArray[0]
        var table: String = srvRspArray[1] + srvRspArray[2]
        var hole_cards: Int = Int(srvRspArray[3])!
        let strLCard: String = srvRspArray[4]
        let strRCard: String = srvRspArray[5]
        
        //println("\(device) \(table) \(hole_cards) \(strLCard) \(strRCard)")
        
        print("\(strLCard) and \(strRCard)")
        
        return (strLCard, strRCard)
    }
    
    func updateCardImages(strLCard: String, strRCard: String) {
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

