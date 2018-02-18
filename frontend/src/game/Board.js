import React from 'react';
import { graphql } from 'react-apollo';
import {compose, pure, branch, renderComponent, lifecycle, withState} from 'recompose';
import JoinGame from './JoinGame';
import BidPhase from './BidPhase';
import { gameQuery, subscribeToChange } from "../api/game";
import {DebugPlayerSwitch} from './DebugPlayerSwitch';

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

export const Board = enhance(({ player, setPlayer, game: { uuid, phase, players }}) => {
    const p = player ? <div>{player.uuid} - {player.name} - {player.spot}</div> : <div></div>;
    return (
      <div>
          <h1>You are</h1>
          {p}
          <DebugPlayerSwitch setPlayer={setPlayer} />
          <h1>Game</h1>
          <div>{uuid}</div>
          <h2>Other Players</h2>
          <ul>
            {players.map(({name, uuid, spot, isDealer}) => <li key={spot}>{uuid} - {name} - {spot}{isDealer ? ' - DEALER': ''}</li>)}
          </ul>
          { phase === 'INITIAL'
              ? <JoinGame playerUuid={player ? player.uuid : null} onGameJoined={setPlayer} gameUuid={uuid}/>
              : <BidPhase playerUuid={player ? player.uuid : null} gameUuid={uuid} />
          }

      </div>
    );
});

export default Board;
