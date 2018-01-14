const NodeCache = require( "node-cache" );
const util = require('util');

const gameCache = new NodeCache( { stdTTL: 24 * 60 * 60, checkperiod: 60 } );

module.exports = {
    set: util.promisify(gameCache.set),
    get: util.promisify(gameCache.get),
};