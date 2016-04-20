# TODO

-   clear(client)

-	consume(client, state)

-	?    convert_time(inTime)

-   current_status(client)

-	?    get_generic_info(status)

-   mpdplay(client, n)

-   next(client)

-   pause(client)

-   play(client, arg)

-   previous(client)

-   print_playlist(client) -cambionome

-   random(client, state)

-   search(searchItem, -Blocation, argFilter)

-   seek(client, state, status)

-   seek_track(client, n, status, mode='s')


-   shuffle(client)

-   single(client, state)

-    stop(client)

-   swap(client, a, b)

-   update(client)

- 	filter

- 	add

- Pickle per il database

# Da finire:
- shell (URGENTE)

# Da implementare:
 - ? opzioni su file di config (vedi bot promemoria)


PERFORMANCE:
- non concatenare con +, usa .join ()
- usa map invece di append per le liste, o comprehension, http://www.python.org/doc/essays/list2str/
- avoid dots, usa tipo upper = str.upper, usa local variables
- pensa agli import
