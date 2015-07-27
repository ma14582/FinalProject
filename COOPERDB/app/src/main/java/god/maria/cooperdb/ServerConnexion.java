/*
 ==============================================================================================
 This class stands the connexion with the server, getting the result according to the service
 passed as a parameter
 ==============================================================================================
*/

package god.maria.cooperdb;

import org.json.JSONException;
import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;

public class ServerConnexion {

    // Server connexion and getting the result
    public String connexionServer(String service) throws JSONException {
        StringBuilder inputStringBuilder = new StringBuilder();
        String uri = "http://10.0.2.2:3000/" + service;
        String result = null;
        URL url = null;
        try {
            url = new URL(uri);
            HttpURLConnection connection = (HttpURLConnection) url.openConnection();
            connection.setRequestMethod("GET");
            connection.setRequestProperty("Accept", "application/json");
            InputStream data = connection.getInputStream();
            connection.disconnect();
            BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(data, "UTF-8"));
            String line = bufferedReader.readLine();
            while (line != null) {
                inputStringBuilder.append(line);
                inputStringBuilder.append('\n');
                line = bufferedReader.readLine();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        result = inputStringBuilder.toString();
        return result;
    }
}
