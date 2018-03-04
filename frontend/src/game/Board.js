import React from 'react';
import { compose, pure, withState } from 'recompose';
import PlayerCards from './PlayerCards';
import TableCards from './TableCards';
import Scores from './Scores';
import Contract from './Contract';
import DebugPlayerSwitch from './component/DebugPlayerSwitch';
import {enhanceWithGame} from "../api";
import { OtherPlayerHand } from './component/OtherPlayerHand';
import "./Board.css";

const testPlayer = { uuid: "1", name: "north" };

const enhance = compose(
    enhanceWithGame((game) => {
        const { uuid, phase } = game;
        return { phase, gameUuid: uuid };
    }),
    withState('player', 'setPlayer', testPlayer),
    pure
);

export const Board = enhance(({ player, setPlayer, gameUuid, phase }) => {
    return (
    <div className="board">
        <div className="board__border">
            <div className="board__table-cards">
                <TableCards playerUuid={player.uuid} gameUuid={gameUuid} />
            </div>
            <div className="board__contract">
                <Contract playerUuid={player.uuid} gameUuid={gameUuid} />
            </div>
            <div className="board__scores">
                <h1>Game</h1>
                <div>{ gameUuid }</div>
                <Scores playerUuid={player.uuid} gameUuid={gameUuid} />
            </div>
            <div className="board__bottom-player">
                <h1>You are</h1>
                {player ? <div>{player.uuid} - {player.name} - {player.spot}</div> : <div></div>}
                <PlayerCards playerUuid={player.uuid} gameUuid={gameUuid} />
            </div>
            <div className="board__top-player">
                <OtherPlayerHand orientation="top" cardCount={5} />
            </div>
            <div className="board__left-player">
                <OtherPlayerHand orientation="left" cardCount={5} />
            </div>
            <div className="board__right-player">
                <OtherPlayerHand orientation="right" cardCount={5} />
            </div>
            <div className="board__debug">
                <h2>Debug player switch</h2>
                <DebugPlayerSwitch setPlayer={setPlayer} />
            </div>
      </div>
    </div>
    );
});

export default Board;
