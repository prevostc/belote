import React from 'react';
import { compose, pure } from "recompose";
import Card from './component/Card';
import {enhanceWithGame} from "../api";

const enhance = compose(
    enhanceWithGame((game) => ({
        table: game.table,
    })),
    pure
);

export const TableCards = enhance(({ table }) => {
    return (
      <div>
          <h2>table</h2>
          <ol>
              {table.map(({color, motif}) => <li key={`${color}-${motif}`}>
                  <Card color={color} motif={motif} />
              </li>)}
          </ol>
      </div>
    );
});

export default TableCards;
