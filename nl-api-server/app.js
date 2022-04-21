const express = require("express");
const nearAPI = require("near-api-js");
const { connect, keyStores, KeyPair, transactions, providers } = nearAPI;

// Receive server configuration
const config = require("./config/config")();
const port = config.port;

// Init NEAR requirements
const provider = new providers.JsonRpcProvider(config.near_config.nodeUrl);

// Initialize Express
const app = express();

// Initialize express middlewares
app.use(express.json());

// Helper functions
async function generateKeystore(account_id = null, private_key = null) {
  const keyStore = new keyStores.InMemoryKeyStore();

  if (account_id && private_key) {
    const PRIVATE_KEY = req.body.private_key;
    const account_id = req.body.account_id;
    const keyPair = KeyPair.fromString(PRIVATE_KEY);

    await keyStore.setKey(config.near_config.networkId, account_id, keyPair);
  }

  return keyStore;
}

/*
    ROUTES
*/
app.get("/", (req, res) => {
  res.send("Near integration server.");
});

// app.post("/wallets", async (req, res) => {
//   // Setup keystore
//   const keyStore = generateKeystore(req.body.account_id, req.body.private_key);

//   // Get required body params
//   const account_id = req.body.account_id;

//   const near_config = {
//     ...config.near_config,
//     keyStore: keyStore,
//   };

//   const near = await connect(near_config);

//   const account = await near.account("exxatest-5.testnet");
//   const account2 = await near.account(account_id);
//   // const new_account = req.body.new_account;
//   console.log(account);
//   console.log(await account2.getAccountBalance());
//   let result = await account2
//     .sendMoney(
//       "lb4.testnet", // receiver account
//       "1000000000000000000000000" // amount in yoctoNEAR
//     )
//     .catch((error) => error);
//   console.log(result);
//   res.send("ack");
// });

// VIEW - Get account state
app.get("/wallets/:wallet_id", async (req, res) => {
  const wallet_id = req.params.wallet_id;

  const keyStore = generateKeystore();

  const near_config = {
    ...config.near_config,
    keyStore: keyStore,
  };

  const near = await connect(near_config);

  await near
    .account(wallet_id)
    .then(async (account) => {
      return res.status(200).send(await account.state());
    })
    .catch((error) => res.status(404).send(error));
});

// VIEW - Get account balance
app.get("/wallets/:wallet_id/balance", async (req, res) => {
  const wallet_id = req.params.wallet_id;

  const keyStore = generateKeystore();

  const near_config = {
    ...config.near_config,
    keyStore: keyStore,
  };

  const near = await connect(near_config);

  await near
    .account(wallet_id)
    .then(async (account) => {
      return res.status(200).send(await account.getAccountBalance());
    })
    .catch((error) => res.status(404).send(error));
});

// VIEW - Call View contract function
app.get("/contract/:contract_id/:function_name", async (req, res) => {
  const keyStore = generateKeystore();

  const near_config = {
    ...config.near_config,
    keyStore: keyStore,
  };

  const near = await connect(near_config);

  const contract_id = req.params.contract_id;
  const function_name = req.params.function_name;
  const contract_account = await near.account(contract_id);

  const result = await contract_account
    .viewFunction(contract_id, function_name, req.query)
    .catch((error) => {
      console.log(error);
      res.status(500).send("Invalid function call or parameters.");
    });

  res.send(result);
});

// CALL - Call contract function
app.post("/contract/function_call", async (req, res) => {
  const contract_id = req.body.contract_id;
  const function_name = req.params.function_name;
  const keyStore = generateKeystore(req.body.private_key, req.body.account_id);

  const near_config = {
    ...config.near_config,
    keyStore: keyStore,
  };

  const near = await connect(near_config);

  const contract_account = await near.account(contract_id);

  const result = await contract_account
    .viewFunction(contract_id, function_name, req.query)
    .catch((error) => {
      console.log(error);
      res.status(500).send("Invalid function call or parameters.");
    });

  res.send(result);
});

// Start server
app.listen(port, () => {
  console.log(`Example app listening on port ${port}`);
});
