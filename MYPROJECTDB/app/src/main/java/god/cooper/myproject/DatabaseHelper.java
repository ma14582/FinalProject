package god.cooper.myproject;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

public class DatabaseHelper extends SQLiteOpenHelper {
    public static final String DATABASE_NAME = "cooper.db";
    public static final String TABLE_NAME = "event_table";
    public static final String COL_1 = "id";
    public static final String COL_2 = "pet_name";
    public static final String COL_3 = "event_type";
    public static final String COL_4 = "date";
    public static final String COL_5 = "time";

    public DatabaseHelper(Context context) {
        super(context, DATABASE_NAME, null, 1);
        SQLiteDatabase db = this.getWritableDatabase();
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        db.execSQL("CREATE TABLE " + TABLE_NAME + " (id INTEGER PRIMARY KEY AUTOINCREMENT, pet_name TEXT, event_type TEXT, date TEXT, time TEXT)");
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        db.execSQL("DROP TABLE IF EXISTS " + TABLE_NAME);
        onCreate(db);
    }

    public boolean insertData(String name, String type, String date, String time){
        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues contentValues = new ContentValues();
        contentValues.put(COL_2, name);
        contentValues.put(COL_3, type);
        contentValues.put(COL_4, date);
        contentValues.put(COL_5, time);
        long result = db.insert(TABLE_NAME, null, contentValues);
        if (result == -1){
            return false;
        } else {
            return true;
        }
    }
    public Cursor getAllData(){
        SQLiteDatabase db = this.getWritableDatabase();
        Cursor result = db.rawQuery("SELECT * FROM " + TABLE_NAME, null);
        return result;
    }
}

