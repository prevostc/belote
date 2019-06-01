import React from 'react';
import { compose, pure } from "recompose";
import Card from './component/Card';
import {enhanceWithGame} from "../api";
import {AspectRatio} from "./component/AspectRatio";
import "./TableCards.css";

const enhance = compose(
    enhanceWithGame((game) => ({
        table: game.table,
    })),
    pure
);

export const TableCards = enhance(({ table }) => {
    return (
      <AspectRatio ratio={1}>
          <div className="table-cards">
          {table.map(({color, motif}) => <Card className="table-cards__card" key={`${color}-${motif}`}color={color} motif={motif} />)}
          </div>
      </AspectRatio>
    );
});

export default TableCards;
