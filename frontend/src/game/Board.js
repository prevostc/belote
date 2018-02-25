import React from 'react';
import { compose, pure, withState } from 'recompose';
import JoinGame from './JoinGame';
import Bidding from './Bidding';
import PlayerCards from './PlayerCards';
import TableCards from './TableCards';
import PlayerList from './PlayerList';
import Scores from './Scores';
import Contract from './Contract';
import DebugPlayerSwitch from './component/DebugPlayerSwitch';
import {enhanceWithGame} from "../api";

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
      <div>
          <h1>You are</h1>
          {player ? <div>{player.uuid} - {player.name} - {player.spot}</div> : <div></div>}
          <h2>Debug player switch</h2>
          <DebugPlayerSwitch setPlayer={setPlayer} />
          <h1>Game</h1>
          <div>{ gameUuid }</div>
          <Scores playerUuid={player.uuid} gameUuid={gameUuid} />
          <PlayerList playerUuid={player.uuid} gameUuid={gameUuid} />
          <Contract playerUuid={player.uuid} gameUuid={gameUuid} />
          <TableCards playerUuid={player.uuid} gameUuid={gameUuid} />
          {((phase) => {
              switch (phase) {
                  case 'INITIAL':
                      return <JoinGame playerUuid={player ? player.uuid : null} onGameJoined={setPlayer} gameUuid={gameUuid} />;
                  case 'BIDDING':
                      return <Bidding playerUuid={player.uuid} gameUuid={gameUuid} />
                  case 'PLAYING':
                      return <PlayerCards playerUuid={player.uuid} gameUuid={gameUuid} />
                  default:
                      return <div>Phase {phase}</div>
              }
          })(phase)}
      </div>
    );
});

export default Board;
