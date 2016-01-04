-module(watcher).
        -import(sensor,[sense/2]).
        -export([main/0,watch/2]).

        main()   -> {ok, [Nsensors]} = io:fread("Number of sensors to spawn: ", "~d"),
                    StartID = 0,
                    watch(StartID,Nsensors).

        watch(ID,Nsensors) -> watchHelp(ID,Nsensors,[]),
                              io:fwrite("Spawned new watcher with PID ~w\n",[self()]).

        watchHelp(_,0,[]) -> noop;

        watchHelp(_,0,SenseList)  -> printTupleList(SenseList),
                                     dispatcher(SenseList);

        watchHelp(ID,Nsensors,SenseList) -> 
                    {Pid, _} = spawn_monitor(sensor,sense,[ID,self()]),
                    %loop until no more sensors to spawn
                    if
                      %crazy things you have to do ensure spawning new watcher precisely every 10 rounds%
                      ( (ID < 10) and (ID rem 9 == 0) and (ID /= 0) ) or ((ID>10) and ( (ID+1) rem 10 == 0)) ->
                           _ = spawn(watcher,watch,[ID+1,Nsensors-1]),
                           watchHelp( ID, 0, [{ID,Pid} | SenseList] );

                      true ->
                           watchHelp( ID + 1, Nsensors - 1, [{ID,Pid} | SenseList] )
                    end.

        dispatcher(SenseList) -> 
                     receive 
                       {SensorID,Measurement} ->
                          io:fwrite("Sensor ~w reports Measurement ~w\n",[SensorID,Measurement]),
                          Updated = SenseList;
                          
                       {'DOWN', _, process, DeadPid, {SensorID,Reason}} ->
                                   io:fwrite("Sensor ~w died because of ~w\n",[SensorID,Reason]),
                                   %respawn sensor with same ID, new Pid
                                   {NewPid, _} = spawn_monitor(sensor,sense,[SensorID,self()]),
                                   Clean = SenseList -- [{SensorID,DeadPid}],
                                   Updated = [{SensorID,NewPid} | Clean],
                                   io:fwrite("!!!!!!!!Updated list:\n"),
                                   printTupleList(Updated)
                     end,
                     dispatcher(Updated).

        printTupleList(TupList) -> Printme = lists:flatten(io_lib:format("~p", [TupList])),
                                   io:fwrite("~s\n",[Printme]).