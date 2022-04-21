const express = require("express");
const nearAPI = require("near-api-js");
const { connect, keyStores, KeyPair, transactions, providers, utils } = nearAPI;

// Receive server configuration
const config = require("./config/config")();
const port = config.port;

// Init NEAR requirements
const provider = new providers.JsonRpcProvider(config.near_config.nodeUrl);

// Initialize Express
const app = express();

// Initialize middlewares
app.use(express.json());

app.use(async (req, res, next) => {
  const account_id = req.body.account_id;
  const private_key = req.body.private_key;

  const keyStore = new keyStores.InMemoryKeyStore();
  if (account_id && private_key) {
    const keyPair = KeyPair.fromString(private_key);

    await keyStore
      .setKey(config.near_config.networkId, account_id, keyPair)
      .catch((error) => next(error));
  }
  const near_config = {
    ...config.near_config,
    keyStore: keyStore,
  };

  const near = await connect(near_config).catch((error) => next(error));

  req.near = near;

  next();
});

// Helper functions

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
  const near = req.near;

  const wallet_id = req.params.wallet_id;

  await near
    .account(wallet_id)
    .then(async (account) => {
      return res.status(200).send(await account.state());
    })
    .catch((error) => res.status(404).send(error));
});

// VIEW - Get account balance
app.get("/wallets/:wallet_id/balance", async (req, res) => {
  const near = req.near;

  const wallet_id = req.params.wallet_id;

  await near
    .account(wallet_id)
    .then(async (account) => {
      return res.status(200).send(await account.getAccountBalance());
    })
    .catch((error) => res.status(404).send(error));
});

// VIEW - Call View contract function
app.get("/contract/:contract_id/:function_name", async (req, res) => {
  const near = req.near;

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
  const near = req.near;

  const account_id = req.body.account_id;
  const contract_id = req.body.contract_id;
  const function_name = req.body.function_name;
  const params = req.body.params || {};
  const gas = req.body.gas || "300000000000000";
  const attached_deposit =
    (req.body.deposit && utils.format.parseNearAmount(req.body.deposit)) ||
    utils.format.parseNearAmount("0.1");

  const signer_account = await near.account(account_id);
  console.log(function_name);
  const result = await signer_account
    .functionCall({
      contractId: contract_id,
      methodName: function_name,
      args: params,
      gas: gas,
      attachedDeposit: attached_deposit,
    })
    .catch((error) => {
      console.error(error);
      res.status(500).send(error);
    });

  res.send(result);
});

// Start server
app.listen(port, () => {
  console.log(`Example app listening on port ${port}`);
});
