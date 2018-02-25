import React from 'react';
import { compose, pure } from "recompose";
import {enhanceWithGame} from "../api";

import Color from './component/Color';

const enhance = compose(
    enhanceWithGame((game) => ({
        scores: game.scores,
    })),
    pure
);

export const Scores = enhance(({ scores }) => {
    return (
      <div>
          <h2>Scores</h2>
          <ul>
              {scores
                  .map(({ contract, winner }, i) => (
                      <li key={i}>
                          {winner} - <Color color={contract.trump}/> - {contract.value} - {contract.player.spot} - {contract.player.team}
                      </li>)
                  )}
          </ul>
      </div>
    );
});

export default Scores;
