This is a REST API, secured by HTTPS.

POST and PUT requests send the NEAR wallet credentials, GET requests don't.

# Installation

Inside the `nl-api-server` folder, run the following commands:

```bash
npm install
npm start
```

Server can also be run with:

```bash
node app.js
```

# Structure

- Folder `config` contains configuration data: environment, NEAR configuration, port, OPTIONAL contract_name.
- Folder `utils` contains helper functions.
- `app.js` contains main code and routes. **TODO** - improve folder organisation

# Core API

|      |                                                        |                                                                                                                                  |
| ---- | ------------------------------------------------------ | -------------------------------------------------------------------------------------------------------------------------------- |
| GET  | /wallets/<wallet_id>                                   | Get basic info about a wallet (at least check if it exists)                                                                      |
| GET  | /wallets/<wallet_id>/balance                           | Get specific info about a wallet: its balance                                                                                    |
| POST | /contract                                              | Deploy a contract                                                                                                                |
| POST | /contract/call_function                                | Call a modifying function on a contract -- params are found in request body                                                      |
| GET  | /contract/:contract_id/:function_name?<args>           | Call a non modifying function on a contract                                                                                      |
| GET  | /contract/<contract_id>/transactions                   | List previous function calls on a contract                                                                                       |
| GET  | /contract/<contract_id>/transactions/<transactions_id> | Get info about a specific function call                                                                                          |
| GET  | /keypair                                               | Generates a public-key pair using Near API and returns it                                                                        |
| GET  | /sign_url                                              | Returns a sign_url to be approved by the user. More info [here](https://github.com/near-examples/near-api-rest-server#sign_url). |

**TODO**: Add API documentation - could use Swagger for this.

### POST - Call function

Call function endpoint accepts the following (example) parameters inside its **body**:

```json
{
  "account_id": "lb4.testnet",
  "private_key": "<private_key>",
  "contract_id": "exxaverse-paras-test.testnet",
  "function_name": "nft_mint",
  "params": {<args needed by the specific function called, dict object>},
  "gas": <prepaid_gas>,
  "deposit": <deposit in NEAR>
}
```

# Optional high level API

|     |                                      |                                                                                                                           |
| --- | ------------------------------------ | ------------------------------------------------------------------------------------------------------------------------- |
| GET | /collectibles/<collectible_id>       | Get info about a specific NFT, the id would include the contract_id, and it needs to call query functions on the contract |
| GET | /wallets/<wallet_id>/collectibles    | List the NFTs owned by a wallet, needs to know or send a list of contracts on which the listed collectibles are defined   |
| PUT | /collectibles/<collectible_id>/owner | Transfer ownership of an NFT, needs to call functions on the contract                                                     |

```

```
