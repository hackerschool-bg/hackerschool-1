package mtsv.test.eventful;

import java.util.Date;

import com.evdb.javaapi.APIConfiguration;
import com.evdb.javaapi.EVDBAPIException;
import com.evdb.javaapi.EVDBRuntimeException;
import com.evdb.javaapi.data.SearchResult;
import com.evdb.javaapi.data.request.EventSearchRequest;
import com.evdb.javaapi.operations.EventOperations;

/*
	Needs the Eventful Java API on the classpath
*/

public class EventfulFetchMain {

	private static final int THREAD_COUNT = 10;
	
	private static EventSearchRequest createBaseRequest() {
		EventSearchRequest esr = new EventSearchRequest();
		esr.setLocation("US");
		esr.setPageSize(100);
		return esr;
	}

	public static void main(String[] args) {
		EventFetchThread[] threads = new EventFetchThread[THREAD_COUNT];
		for (int i=0;i<THREAD_COUNT;i++) {
			threads[i] = new EventFetchThread();
		}

		APIConfiguration.setEvdbUser("mogmog444");
		APIConfiguration.setEvdbPassword("mogmog444");
		APIConfiguration.setApiKey("hBvQkZXjNn5B6Xnf");

		EventOperations eo = new EventOperations();
		SearchResult sr = null;
		try {
			sr = eo.search(createBaseRequest());
		} catch (EVDBRuntimeException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (EVDBAPIException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		int numPages = sr.getPageCount();
		System.out.println("Total pages: " + numPages);
		
		for (int i=0;i<numPages;i++) {
			EventSearchRequest esr = createBaseRequest();
			esr.setPageNumber(i+1);
			threads[i%THREAD_COUNT].addRequest(esr);
		}
		System.out.println(new Date());
		for (int i=0;i<THREAD_COUNT;i++) {
			threads[i].start();
		}
		
		for (int i=0;i<THREAD_COUNT;i++) {
			try {
				threads[i].join();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		System.out.println(new Date());
	}

}
