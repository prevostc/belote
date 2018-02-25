import React from 'react';
import { compose, pure } from "recompose";
import {enhanceWithGame} from "../api";

const enhance = compose(
    enhanceWithGame((game) => ({
        players: game.players,
    })),
    pure
);

export const PlayerList = enhance(({ players }) => {
    return (
      <div>
          <h2>Other Players</h2>
          <ul>
              {[...players]
                  .sort(({ uuid: a }, { uuid: b }) => a.localeCompare(b))
                  .map(({name, uuid, spot, isDealer, actionNeeded}) => (
                      <li key={spot}>
                          {uuid} - {name} - {spot}{isDealer ? ' - DEALER': ''}{actionNeeded ? ' - ACTION_NEEDED': ''}
                      </li>)
                  )}
          </ul>
      </div>
    );
});

export default PlayerList;
