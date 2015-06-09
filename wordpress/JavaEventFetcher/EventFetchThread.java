package mtsv.test.eventful;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.util.LinkedList;
import java.util.List;

import com.evdb.javaapi.EVDBAPIException;
import com.evdb.javaapi.EVDBRuntimeException;
import com.evdb.javaapi.data.Event;
import com.evdb.javaapi.data.SearchResult;
import com.evdb.javaapi.data.request.EventSearchRequest;
import com.evdb.javaapi.operations.EventOperations;

public class EventFetchThread extends Thread {

	private LinkedList<EventSearchRequest> requests;
	private EventOperations op;
	
	public EventFetchThread() {	
		op = new EventOperations();
		requests = new LinkedList<EventSearchRequest>();
	}
	
	public void addRequest(EventSearchRequest esr) {
		requests.add(esr);
	}
	
	@Override
	public void run() {
		for (EventSearchRequest esr : requests) {
			handleRequest(esr);
		}
	}
	
	private void handleRequest(EventSearchRequest esr) {
		SearchResult sr = null;
		try {
			sr = op.search(esr);
		} catch (EVDBRuntimeException e) {
			System.err.println("\n\n"+esr.getPageNumber()+": " + e.getClass().getName());
		} catch (EVDBAPIException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		if (sr == null) return;
		
		List<Event> events = sr.getEvents();
		Connection conn = null;
		try {
			conn = DriverManager.getConnection("jdbc:mysql://localhost?user=root&password=root");
			conn.setAutoCommit(false);
			for (Event e : events) {
				PreparedStatement ps = conn
						.prepareStatement("INSERT IGNORE INTO wordpress.events (id, name, description, startTime, url, latitude, longitude, city, region, country) VALUES (?,?,?,?,?,?,?,?,?,?)");
				ps.setString(1, e.getSeid());
				ps.setString(2, e.getTitle());
				ps.setString(3, e.getDescription());
				ps.setDate(4, new java.sql.Date(e.getStartTime().getTime()));
				ps.setString(5, e.getURL());
				ps.setDouble(6, e.getVenueLatitude());
				ps.setDouble(7, e.getVenueLongitude());
				ps.setString(8, e.getVenueCity());
				ps.setString(9, e.getVenueRegion());
				ps.setString(10, e.getVenueCountry());
				ps.execute();
			}
			conn.commit();
			System.out.print(esr.getPageNumber());
			if (esr.getPageNumber() % 50 == 0) {
				System.out.println();
			} else {
				System.out.print(" ");
			}
		} catch (SQLException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		} finally {
			try {
				if (conn != null) conn.close();
			} catch (SQLException e) {
				e.printStackTrace();
			}
		}
	}

}
