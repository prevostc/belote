import React from 'react';
import { compose, pure, withState } from 'recompose';
import JoinGame from './JoinGame';
import Bidding from './Bidding';
import Playing from './Playing';
import Card from './component/Card';
import Color from './component/Color';
import DebugPlayerSwitch from './component/DebugPlayerSwitch';
import {enhanceWithGame} from "../api";

const testPlayer = { uuid: "1", name: "north" };

const enhance = compose(
    enhanceWithGame((game) => {
        const { uuid, table, phase, players, contract } = game;
        return { table, phase, players, contract, gameUuid: uuid };
    }),
    withState('player', 'setPlayer', testPlayer),
    pure
);

export const Board = enhance(({ player, setPlayer, gameUuid, table, phase, players, contract }) => {
    const p = player ? <div>{player.uuid} - {player.name} - {player.spot}</div> : <div></div>;
    const renderContract = (contract) => contract === null
        ? (<span></span>)
        : <div><Color color={contract.trump}/> - {contract.value} - {contract.player.spot} - {contract.player.team}</div>
    const renderPhase = (phase) => {
        switch (phase) {
            case 'INITIAL':
                return <JoinGame playerUuid={player ? player.uuid : null} onGameJoined={setPlayer} gameUuid={gameUuid} />;
            case 'BIDDING':
                return <Bidding playerUuid={player.uuid} gameUuid={gameUuid} />
            case 'PLAYING':
                return <Playing playerUuid={player.uuid} gameUuid={gameUuid} />
            default:
                return <div>Phase {phase}</div>
        }
    };

    return (
      <div>
          <h1>You are</h1>
          {p}
          <h2>Debug player switch</h2>
          <DebugPlayerSwitch setPlayer={setPlayer} />
          <h1>Game</h1>
          <div>{ gameUuid }</div>
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
          <h2>contract</h2>
           {renderContract(contract)}
          <h2>table</h2>
          <ol>
              {table.map(({color, motif}) => <li key={`${color}-${motif}`}>
                  <Card color={color} motif={motif} />
              </li>)}
          </ol>
          { renderPhase(phase) }
      </div>
    );
});

export default Board;
