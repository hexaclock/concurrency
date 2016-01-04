import java.util.HashMap;
import java.util.Set;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.Queue;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Collections;
import java.util.Comparator;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.locks.*;
import java.util.concurrent.Semaphore;


public class TokenCount2
{
    private static final HashMap<String, Integer> tokenFreq = new HashMap<String, Integer>();
    //private static final Map<String, Integer> tokenFreq = Collections.synchronizedMap(new HashMap<String,Integer>());
    private static final BlockingQueue<Page> abq = new ArrayBlockingQueue<>(100);
    //private static final ReentrantLock maplock = new ReentrantLock();
    //private static final Object simplewritelock = new Object();
    //private static final Object simplereadlock = new Object();
    //private static final ReentrantLock rwlock = new ReentrantLock();
    //private static final Lock rlock = rwlock.readLock();
    //private static final Lock wlock = rwlock.writeLock();

    private static synchronized void countToken(String tok)
        {
            Integer currentCount;
            
            currentCount = tokenFreq.get(tok);
            if (currentCount == null)
            {
                tokenFreq.put(tok,1);
            }
            else
            {
                tokenFreq.put(tok,currentCount+1);
            }
        }


    public static void main(final String[] args) throws Exception
        {
	if (args.length != 2)
	    {
		System.out.println("usage: java TokenCount2 number-of-pages XML-file");
		System.exit(0);
	    }
	final Integer numPages = Integer.parseInt(args[0]);

        ExecutorService cachedPool = Executors.newCachedThreadPool();
	// print number of available processors
        final int numCPUs = Runtime.getRuntime().availableProcessors();
	System.out.println(numCPUs + " available processors");

        //setup thread queue
        BlockingQueue<Future> futQ = new ArrayBlockingQueue<>(numCPUs-1);

        //setup threads using Java's anonymous class feature
        Thread Producer = new Thread()
            {
                private PoisonPill pp = new PoisonPill();
                
                public void run()
                    {
                        // parse XML into pages
                        Iterable<Page> allPages = new Pages(numPages, args[1]);
                        for (Page pg: allPages)
                        {
                            try {
                                abq.put(pg);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                        //place poison pills
                        for (int i=0; i<numCPUs-1; ++i)
                        {
                            try {
                                abq.put(pp);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                    }
            };
       
        int i;
	/* begin timed code ... */
	final long before = System.nanoTime();
        Producer.start();
        for (i=0; i<numCPUs-1; ++i)
        {
            try {
                futQ.put(cachedPool.submit( new Runnable() {
                        @Override
                        public void run()
                            {
                                Page pg = null;
                                Iterable<String> allTokens;
                                
                                while(true)
                                {
                                    try {
                                        pg = abq.take();
                                    }
                                    catch (InterruptedException e){
                                        e.printStackTrace();
                                    }
                                    if (pg.isPoisonPill())
                                        return;
                                    allTokens = new Words(pg.getText());
                                    for (String s: allTokens)
                                    {
                                        countToken(s);
                                    }
                                }
                            }
                    }));
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        
        for (i=0; i<numCPUs-1; ++i)
        {
            try {
                futQ.take().get();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        
        cachedPool.shutdown();
        Producer.join();
        final long after = System.nanoTime();
	/* ... end  timed code */
	
	System.out.println("Time to process " + numPages + " pages = " + (after - before)/1000000 + " milliseconds");
	
	// sort tokenFreq by value & print top 30 most common tokens
	Set<Entry<String, Integer>> entries = tokenFreq.entrySet();
	ArrayList<Entry<String, Integer>> list = new ArrayList<Entry<String, Integer>>(entries);
	Collections.sort(list, new Comparator<Map.Entry<String, Integer>>()
			 {
			     public int compare(Map.Entry<String, Integer> obj1, Map.Entry<String, Integer> obj2)
			     {
				 return (obj2.getValue()).compareTo(obj1.getValue());
			     }
			 } );
	for(i=0; i<30; i++)
	    System.out.println(list.get(i).getKey() + " appears " + list.get(i).getValue() + " times");
    }

}
