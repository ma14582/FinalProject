package god.maria.cooperdb;

import android.database.Cursor;

import java.util.HashSet;
import java.util.Set;

/**
 * Created by maria on 26/07/2015.
 */
public class EventCache {

    /*private final Set<Integer> serverIds = new HashSet<>();
    private DatabaseHelper myDb;

    public EventCache(DatabaseHelper myDb) {
        this.myDb = myDb;
    }

    public void init() {
        Cursor resultQuery = myDb.getServerIdAndroid();
        while (resultQuery.moveToNext()) {
            int index = 0;
            this.serverIds.add(resultQuery.getInt(index++));
        }
        resultQuery.close();
    }

    public boolean contains(Integer value) {
        return this.serverIds.contains(value);
    }

    public void insert(Integer value) {
        this.serverIds.add(value);
    }

    public void remove(Integer value) {
        this.serverIds.remove(value);
    }*/
}
