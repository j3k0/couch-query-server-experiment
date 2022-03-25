# Experimental CouchDB Query Server

I've built a C and TypeScript experimental CouchDB query server to run some benchmarks.

Code shared in this directory.

## Benchmarks 

For my benchmark, I use a database with:

- size=234.1 MB
- `doc_count`=41634
- `doc_del_count`=62044
- `map`: `function(doc){emit('key', 'value');}`
- `reduce`: `function(){return "reduced";}`

I update the design document, make sure the indexer runs and go in the activity page to measure the time it takes to complete indexing.

I compare below:

- `couchjs` -  map/reduce handled by the couchjs javascript query server.
- `nodejs` - map/reduce handled using the query server found in the `ts` directory (running with nodejs).
- `c` - map/reduce handled by a native program in C, found in file `extracouch.c`.

### Data

Here's the time it took in different runs.

| qs | run1 | run2 | run3 |
|---|---|---|---|
| couchjs | 124s | 130s |
| nodejs | 83s | 92s | |
| c | 75s | 103s | 79s |

In this synthetic test, we can hope for maximum 33% to 40% performance 
improvement by using a custom query server. However notice that the reduce 
function is as simple as it can be, compiling the reduce function at each call 
is what is expected to slows down `couchjs`.

For the record, I also ran the above using the built-in `_count` reducer.

| qs | run |
|---|---|
| couchjs | 91s |
| nodejs | 102s |
| c | 86s |

It doesn't make that much of a difference in this case, expect for the couchjs 
server. I probably ran the nodejs test at the wrong time, because it doesn't 
make sense it's slower that other runs, anyway I don't expect significant 
improvement here.

### Way forward

 - A real C implementation would be too much effort for no substantial 
   improvement.

 - A NodeJS implementation will offer benefits, especially by allowing us to 
   integrate a complex mapper function as a built-in (so the full code don't 
   have to be sent with each call to the reducer).

 - The existing CouchJS can be improved: "take a look at state.js - this is 
   where compiled functions are stored - maybe adding additional property 
   there, and storing your compiled reduce would help?  Instead State.funs = 
   [], you could use State.reduceFuns = {} (JS array like object instead of 
   array, so you could store the compiled functions there by their text) - this 
   is just a concept, and I have no idea if this won't affect couchjs memory, 
   but I don't think so..." - the problem remains that very complex reduce 
   functions will have to go through the wire for every document. A change in 
   the query protocol would be required to allow registering the reduce 
   function before calling reduce.

 - Another improvement could be to pass in data between couch and the query 
   server in more optimised ways. This would be a change inside CouchDB, out of 
   reach for me at the moment.

