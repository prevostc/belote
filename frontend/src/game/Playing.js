import React from 'react';
import { compose, pure } from "recompose";
import Card from './component/Card';
import {connectPlayCardMutation, enhanceWithGame} from "../api";

const enhance = compose(
    connectPlayCardMutation,
    enhanceWithGame((game) => ({
        cards: game.cards,
    })),
    pure
);

export const Playing = enhance(({ cards, playCard }) => {
    return (
      <div>
          <h1>Card</h1>
          <ol>
            {cards.map(({color, motif, canBePlayed}) => <li key={`${color}-${motif}`}>
                <Card color={color} motif={motif} />
                {canBePlayed ? <button onClick={() => playCard(color, motif)}>Play this</button> : ''}
            </li>)}
          </ol>
      </div>
    );
});

export default Playing;
