package god.maria.cooper;

import org.junit.Before;
import org.junit.Test;
import org.mockito.Mock;
import static org.mockito.Mockito.*;

public class EventActivityTest {

    private EventActivity eventActivity;
    @Mock
    private DatabaseHelper databaseHelper;

    @Before
    public void setup() {
        this.eventActivity = new EventActivity(this.databaseHelper);
    }

    @Test
    public void testInsertData() throws Exception {


    }
}