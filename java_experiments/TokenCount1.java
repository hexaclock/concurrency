import java.util.HashMap;
import java.util.Set;
import java.util.ArrayList;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Collections;
import java.util.Comparator;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;


public class TokenCount1
{
    private static final HashMap<String, Integer> tokenFreq = new HashMap<String, Integer>();
    private static final BlockingQueue<Page> abq = new ArrayBlockingQueue<>(100);

    public static void main(final String[] args) throws Exception
    {
	if (args.length != 2)
	    {
		System.out.println("usage: java TokenCount1 number-of-pages XML-file");
		System.exit(0);
	    }
	final Integer numPages = Integer.parseInt(args[0]);
	
	// print number of available processors
	System.out.println(Runtime.getRuntime().availableProcessors() + " available processors");

        //setup threads using Java's anonymous class feature
        Thread Producer = new Thread()
            {
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
                        try {
                            abq.put(new PoisonPill());
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
            };
        Thread Consumer = new Thread()
            {
                public void run()
                    {
                        Page pg;
                        while (true)
                        {
                            try {
                                pg = abq.take();
                                if (pg.isPoisonPill())
                                {
                                    return;
                                }
                                Iterable<String> allTokens = new Words(pg.getText());
                                for (String s: allTokens)
                                    countToken(s);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                    }

                private void countToken(String tok)
                    {
                        Integer currentCount = tokenFreq.get(tok);
                        if (currentCount == null)
                            tokenFreq.put(tok, 1);
                        else
                            tokenFreq.put(tok, currentCount + 1);
                    }
            };
            

	/* begin timed code ... */
	final long before = System.nanoTime();
        Producer.start();
        Consumer.start();
        Consumer.join();
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
	for(int i=0; i<30; i++)
	    System.out.println(list.get(i).getKey() + " appears " + list.get(i).getValue() + " times");
    }

}
