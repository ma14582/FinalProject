/*
 ==============================================================================================
 This class stands the webSocket connexion
 ==============================================================================================
*/

package god.maria.cooperdb;

import com.github.nkzawa.emitter.Emitter;
import com.github.nkzawa.socketio.client.IO;
import com.github.nkzawa.socketio.client.Socket;

import java.net.URISyntaxException;
import java.util.Date;
import java.util.logging.Level;
import java.util.logging.Logger;

public class WebSocketHelper {

    private Socket socket;

    public void connect() {
        try {
            //socket = IO.socket("http://10.0.2.2:3003");
            socket = IO.socket("http://192.168.0.3:3003");
            socket
                    .on(Socket.EVENT_CONNECT_ERROR, new Emitter.Listener() {
                        @Override
                        public void call(Object... args) {
                            System.out.println("connect error");
                        }
                    })
                    .on(Socket.EVENT_CONNECT, new Emitter.Listener() {
                        @Override
                        public void call(Object... args) {
                            socket.emit("update", "hi");
                        }

                    })
                    .on("message", new Emitter.Listener() {
                        @Override
                        public void call(Object... args) {
                            System.out.println("werwer");
                        }
                    })
                    .on(Socket.EVENT_DISCONNECT, new Emitter.Listener() {
                        @Override
                        public void call(Object... args) {
                            System.out.println("disconnect");
                        }

                    });

            socket.connect();
            Date date = new Date();
            while (!socket.connected() && (new Date().getTime() - date.getTime()) < 3000) {

            }
        } catch (URISyntaxException ex) {
            Logger.getLogger(MainActivity.class.getName()).log(Level.SEVERE, null, ex);
        }

    }

    public void emitUpdate(String message) {
        this.emit("update", message);
    }

    private void emit(String method, String message) {
        socket.emit("update", message);
    }

    public void disconnect(){
        socket.disconnect();
    }


}
