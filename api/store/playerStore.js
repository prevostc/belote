const NodeCache = require( "node-cache" );
const util = require('util');

const playerCache = new NodeCache( { stdTTL: 24 * 60 * 60, checkperiod: 60 } );

module.exports = {
    save: (player) => util.promisify(playerCache.set)(player.uuid, player),
    get: async(uuid) => {
        const player = await util.promisify(playerCache.get)(uuid)
        if (! player) {
            throw new Error('Player not found')
        }
        return player;
    },
};