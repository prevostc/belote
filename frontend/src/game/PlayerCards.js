import React from 'react';
import { compose, pure } from "recompose";
import Card from './component/Card';
import {connectPlayCardMutation, enhanceWithGame} from "../api";
import {Hand} from "./component/Hand";

const enhance = compose(
    connectPlayCardMutation,
    enhanceWithGame((game) => ({
        cards: game.cards,
    })),
    pure
);

export const PlayerCards = enhance(({ cards, playCard }) => {
    return <Hand cards={cards} orientation="bottom">
        {({color, motif, canBePlayed}, props) => {
            return <Card {...props} color={color} motif={motif} />;
        }}
    </Hand>;
});

export default PlayerCards;
