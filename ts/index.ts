import * as readline from 'readline';
import { stdin, stdout } from 'node:process';

const pipe = readline.createInterface({ input: stdin, output: stdout });

main();

async function main() {
    pipe.on("line", function lineReceived(input: string): void {
        if (/^[ \t\n]*$/.test(input)) return;
        let dataLine:string[] = [];
        try {
            const data = JSON.parse(`{"line":${input}}`);
            if (data.line?.length) dataLine = data.line;
        }
        catch (err) {
            if (err instanceof Error)
                console.log(JSON.stringify(["error", "parse_error", err.message + " input: \"" + input + "\""]));
        }

        if (dataLine.length > 0) {
            console.log(JSON.stringify(processQuery(dataLine)));
        }
    });
}

type QueryServerState = {
    reduce_limit?: boolean;
    timeout?: number;
    mapFunctions?: string[];
}

let state: QueryServerState = {};
state;

// function exit() { pipe.close(); }

/**
 * Process a query from CouchDB, return the response. 
 * 
 * @param line - parsed line received from CouchDB
 * @returns the response to send to CouchDB
 * 
 * @see https://docs.couchdb.org/en/stable/query-server/protocol.html
 */
function processQuery(line: any[]): any {

    switch (line[0]) {
        case 'reset':
            if (line[1] && typeof line[1] === 'object') {
                state = line[1];
            }
            else {
                state = {};
            }
            return true;

        case 'add_lib':
            return true;
        
        case 'add_fun':
            // in principle, there could be multiple map functions. we just support one here.
            if (!state.mapFunctions) state.mapFunctions = [];
            state.mapFunctions.push(line[1]);
            return true;
        
        case 'map_doc':
            if (!state.mapFunctions) return [];
            const obj = line[1];
            return state.mapFunctions.map((fn:string) => map(fn, obj));
        
        case 'reduce': {
            const funs: string[] = line[1];
            const docs: [key: [key: string, docID: string], value: any][] = line[2];
            return [true, funs.map(fn => reduce(fn, docs))];
        }

        case 'rereduce': {
            const funs: string[] = line[1];
            const docs: any[] = line[2];
            return [true, funs.map(fn => rereduce(fn, docs))];
        }

        case 'ddoc': return true; // not caching anything!

        // case 'shows': return true; unsupported
        // case 'lists': return true; unsupported
        // case 'updates': return true; unsupported
        // case 'filters': return true; unsupported
        // case 'views': return true; unsupported
        // case 'validate_doc_update': return true; unsupported
        // case 'rewrites': return true; unsupported

        default:
            return ["error", "unsupported_command", "command '" + line[0] + "' is not supported by this query server"];
    }
}

function map(fn: string, obj: any) {
    const emits = [['key', 'value']];
    return emits;
}

function reduce(fn: string, objs: any[]) {
    return 'reduced';
}

function rereduce(fn: string, objects: any[]): any {
    return 're-reduced';
}