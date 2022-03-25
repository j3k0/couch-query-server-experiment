# Experiment CouchDB Query Server

I built a C and TypeScript experimental CouchDB query server to run some benchmarks.

Code shared in this directory.

## Benchmarks 

For my benchmark, I use a database with:

- size=234.1 MB
- `doc_count`=41634
- `doc_del_count`=62044
- `map`: `function(doc){emit('key', 'value');}`
- `reduce`: `function(){return "reduced";}`

I update the design document, make sure the indexer runs and go in the activity page to measure the take it takes to complete indexing.

I compare below:

- `couchjs` -  Implemented using the built-in javascript query server.
- `nodejs` - Implemented using the query server found in the `ts` directory.
- `c` - Implemented using the query server in file `extracouch.c`.

### Data

| qs | run1 | run2 | run3 |
|---|---|---|---|
| couchjs | 124s | 130s |
| nodejs | 83s | 92s | |
| c | 75s | 103s | 79s |

In this "ideal" case we can hope for maximum 33% to 40% performance improvement 
by using a custom reducer. However notice that the reduce function is as simple 
as it can be, compiling the reduce function at each call is what slows down 
`couchjs`.

For the record, I also run using the built-in `_count` reducer.

| qs | run |
|---|---|
| couchjs | 91s |
| nodejs | 102s |
| c | 86s |

Doesn't make that much of a difference in this case (maybe I ran the nodejs at 
the wrong time, because it doesn't make sense it's slower that other runs)

### Way forward

A real C implementation would be too much effort for no substantial 
improvement.

A NodeJS implementation will offer benefits, especially by allowing us to 
integrate a complex mapper function as a built-in (so the code don't have to be 
sent with each reduce request).

The existing CouchJS can be improved: "take a look at state.js - this is where 
compiled functions are stored - maybe adding additional property there, and 
storing your compiled reduce would help?  Instead State.funs = [], you could 
use State.reduceFuns = {} (JS array like object instead of array, so you could 
store the compiled functions there by their text) - this is just a concept, and 
I have no idea if this won't affect couchjs memory, but I don't think so..." - 
the problem remains that very complex reduce functions will have to go through 
the wire for every document. A change in the query protocol would be required 
to allow registering the reduce function before calling reduce.

A bigger improvement could be to pass in data between couch and the query 
server in more optimised way? That will be a change inside CouchDB, out of 
reach for me at the moment.

