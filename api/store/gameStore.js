const NodeCache = require( "node-cache" );
const util = require('util');
const { format } = require('../engine');
const gameCache = new NodeCache( { stdTTL: 24 * 60 * 60, checkperiod: 60 } );

module.exports = {
    save: (game) => util.promisify(gameCache.set)(format.getUuid(game), game),
    get: async(uuid) => {
        const game = await util.promisify(gameCache.get)(uuid)
        if (! game) {
            console.log(await util.promisify(gameCache.keys)())
            throw new Error('Game not found')
        }
        return game;
    },
};