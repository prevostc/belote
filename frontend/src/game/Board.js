import React from 'react';
import { graphql } from 'react-apollo';
import { compose, pure, branch, renderComponent, lifecycle, withState } from 'recompose';
import JoinGame from './JoinGame';
import Bidding from './Bidding';
import Playing from './Playing';
import Card from './component/Card';
import Color from './component/Color';
import { gameQuery, subscribeToChange } from "../api/game";
import { DebugPlayerSwitch } from './DebugPlayerSwitch';

const testPlayer = { uuid: "1", name: "north" };

const enhance = compose(
    graphql(gameQuery, {
        props: ({ data }) => {
            const { loading, error, game } = data;
            return ({
                loading,
                error,
                game,
                subscribeToChange: subscribeToChange(data),
            })
        },
        options: ({ uuid }) => ({
            variables: { uuid },
        }),
    }),
    lifecycle({
        componentWillMount() {
            this.props.subscribeToChange({
                uuid: this.props.uuid,
            });
        }
    }),
    branch(
        props => props.loading,
        renderComponent(() => <div>LOADING</div>),
    ),
    branch(
        props => props.error,
        renderComponent(({ error }) => <div>ERROR: {JSON.stringify(error)}</div>),
    ),
    withState('player', 'setPlayer', testPlayer),
    pure
);

export const Board = enhance(({ player, setPlayer, game: { uuid, table, phase, players, contract }}) => {
    const p = player ? <div>{player.uuid} - {player.name} - {player.spot}</div> : <div></div>;
    const renderContract = (contract) => contract === null
        ? (<span></span>)
        : <div><Color color={contract.trump}/> - {contract.value} - {contract.player.spot} - {contract.player.team}</div>
    const renderPhase = (phase) => {
        switch (phase) {
            case 'INITIAL':
                return <JoinGame playerUuid={player ? player.uuid : null} onGameJoined={setPlayer} gameUuid={uuid}/>;
            case 'BIDDING':
                return <Bidding playerUuid={player ? player.uuid : null} gameUuid={uuid} />
            case 'PLAYING':
                return <Playing playerUuid={player ? player.uuid : null} gameUuid={uuid} />
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
          <div>{uuid}</div>
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
