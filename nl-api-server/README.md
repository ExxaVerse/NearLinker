This is a REST API, secured by HTTPS.

POST and PUT requests send the NEAR wallet credentials, GET requests don't.

# Core API

---- --------------------------------------------------------- ---
POST /wallets                                                  Create a wallet, returns credentials
GET  /wallets/<wallet_id>                                      Get basic info about a wallet (at least check if it exists)
GET  /wallets/<wallet_id>/balance                              Get specific info about a wallet: its balance
POST /contract                                                 Deploy a contract
POST /contract/<contract_id>/transactions                      Call a modifying function on a contract
GET  /contract/<contract_id>/<function_name>?param1=value1&... Call a non modifying function on a contract
GET  /contract/<contract_id>/transactions                      List previous function calls on a contract
GET  /contract/<contract_id>/transactions/<transactions_id>    Get info about a specific function call
---

# Optional high level API

---- --------------------------------------------------------- ---
GET  /collectibles/<collectible_id>                            Get info about a specific NFT, the id would include the contract_id, and it needs to call query functions on the contract
GET  /wallets/<wallet_id>/collectibles                         List the NFTs owned by a wallet, needs to know or send a list of contracts on which the listed collectibles are defined
PUT /collectibles/<collectible_id>/owner                       Transfer ownership of an NFT, needs to call functions on the contract
---
