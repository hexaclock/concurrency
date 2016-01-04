-module(sensor).
        -export([sense/2,report/3]).

        sense(ID,Watcher) -> {Mega, Sec, Usec} = now(),
                             random:seed({ID,Usec+Sec,ID+Mega+Usec+Sec}),
                             Measurement = random:uniform(11),
                             report(Measurement,ID,Watcher),
                             Sleeptime = random:uniform(10000),
                             timer:sleep(Sleeptime),
                             sense(ID,Watcher).

        %works better, but takes too long when there are lot of sensors b/c kernel's PRNG blocks
        %sense(ID,Watcher) -> Measurement = crypto:rand_uniform(1,12),
        %                     report(Measurement,ID,Watcher),
        %                     Sleeptime = crypto:rand_uniform(1,10001),
        %                     timer:sleep(Sleeptime),
        %                     sense(ID,Watcher).

        report(11,ID,_) -> exit({ID,anomalous_reading});
        report(Measurement,ID,Watcher)  -> Watcher ! {ID,Measurement}.
