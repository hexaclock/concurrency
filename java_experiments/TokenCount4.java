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
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.locks.ReentrantLock;
import java.util.concurrent.Semaphore;
import java.util.Enumeration;

public class TokenCount4
{
    private static final ConcurrentHashMap<String, Integer> tokenFreq = new ConcurrentHashMap<String, Integer>();
    private static final BlockingQueue<Page> abq = new ArrayBlockingQueue<>(100);
    //private static final ReentrantLock maplock = new ReentrantLock();

    private static void countToken(String tok, Integer minicnt)
        {
            Integer currentCount;
            Integer newCount;
            do {
                currentCount = tokenFreq.get(tok);
                if (currentCount == null)
                {
                    tokenFreq.putIfAbsent(tok,minicnt);
                    return;
                }
                else
                    newCount = currentCount + minicnt;
            } while(tokenFreq.replace(tok,currentCount,newCount) == false);
        }
    
    public static void main(final String[] args) throws Exception
    {
	if (args.length != 2)
	    {
		System.out.println("usage: java TokenCount4 number-of-pages XML-file");
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
                public void run()
                    {
                        PoisonPill pp = new PoisonPill();
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
        //Future<?> fut;
	/* begin timed code ... */
	final long before = System.nanoTime();
        Producer.start();
        for (i=0; i<numCPUs-1; ++i)
        {
            try {
                futQ.put(cachedPool.submit(new Runnable()
            {
                private ConcurrentHashMap<String, Integer> miniTokenFreq = new ConcurrentHashMap<String, Integer>();
                public void run()
                    {
                        Page pg;
                        while (true)
                        {
                            try {
                                pg = abq.take();
                                if (pg.isPoisonPill())
                                {
                                    String tok;
                                    for (Enumeration<String> e = miniTokenFreq.keys(); 
                                         e.hasMoreElements();)
                                    {
                                        tok = e.nextElement();
                                        countToken(tok,miniTokenFreq.get(tok));
                                    }
                                    return;
                                }
                                Iterable<String> allTokens = new Words(pg.getText());
                                for (String s: allTokens)
                                {
                                    miniCountToken(s);
                                }
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            } 
                        }
                    }

                private void miniCountToken(String tok)
                    {
                        Integer currentCount;
                        Integer newCount;
                        do {
                            currentCount = miniTokenFreq.get(tok);
                            if (currentCount == null)
                            {
                                miniTokenFreq.putIfAbsent(tok,1);
                                return;
                            }
                            else
                                newCount = currentCount + 1;
                        } while(miniTokenFreq.replace(tok,currentCount,newCount) == false);
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
        Producer.join();
        cachedPool.shutdown();
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
