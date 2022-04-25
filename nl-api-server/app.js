const express = require("express");
const nearAPI = require("near-api-js");
const { connect, keyStores, KeyPair, transactions, providers, utils } = nearAPI;

// Receive server configuration
const config = require("./config/config")();
const port = config.port;

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

// GET - Get transaction info
app.get(
  "/contract/:contract_id/transactions/:transaction_id",
  async (req, res) => {
    const near = req.near;

    const contract_id = req.params.contract_id;
    const transaction_id = req.params.transaction_id;

    const result = await near.connection.provider
      .txStatus(transaction_id, contract_id)
      .catch((error) => res.status(500).send(error));

    res.send(result);
  }
);

// TODO: GET - Get recent transactions for account
app.get(
  "/contract/:contract_id/transactions/:transaction_id",
  async (req, res) => {
    const near = req.near;

    const contract_id = req.params.contract_id;
    const transaction_id = req.params.transaction_id;

    const result = await near.connection.provider.txStatus(
      transaction_id,
      contract_id
    );

    res.send(result);
  }
);

// GET - Key pair
app.get("/keypair", async (_, res) => {
  const keypair = utils.KeyPair.fromRandom("ed25519");

  result = {
    public_key: keypair.publicKey.toString(),
    private_key: keypair.secretKey,
  };

  res
    .set("Last-Modified", new Date())
    .set("Expires", new Date())
    .set("Cache-Control", "no-cache")
    .set("Pragma", "no-cache")
    .send(result);
});

// POST - Sign URL endpoint
app.post("/sign_url", async (req, res) => {
  let {
    account_id,
    method,
    params,
    deposit,
    gas,
    receiver_id,
    meta,
    callback_url,
    network,
  } = req.body;

  try {
    const keypair = utils.KeyPair.fromRandom("ed25519");

    if (!network) network = "mainnet";
    // Format deposit
    const deposit_value =
      typeof deposit == "string"
        ? deposit
        : utils.format.parseNearAmount("" + deposit);

    // Setup actions
    const actions = [
      method === "!transfer"
        ? transactions.transfer(deposit_value)
        : // In order to use this, the key pair has to be generated beforehand
        // and only send the public key through the call
        method === "add_key"
        ? transactions.addKey(
            params.public_key,
            transactions.functionCallAccessKey(
              params.contract_id,
              params.methodNames,
              utils.format.parseNearAmount("" + params.allowance)
            )
          )
        : transactions.functionCall(
            method,
            Buffer.from(JSON.stringify(params)),
            gas,
            deposit_value
          ),
    ];

    // Provider, block, transactions
    const provider = new providers.JsonRpcProvider({
      url: config.near_config.nodeUrl,
    });
    const block = await provider.block({ finality: "final" });
    const txs = [
      transactions.createTransaction(
        account_id,
        keypair.publicKey,
        receiver_id,
        1,
        actions,
        utils.serialize.base_decode(block.header.hash)
      ),
    ];
    const newUrl = new URL("sign", "https://wallet." + network + ".near.org/");
    newUrl.searchParams.set(
      "transactions",
      txs
        .map((transaction) =>
          utils.serialize.serialize(transactions.SCHEMA, transaction)
        )
        .map((serialized) => Buffer.from(serialized).toString("base64"))
        .join(",")
    );
    newUrl.searchParams.set("callbackUrl", callback_url);
    if (meta) newUrl.searchParams.set("meta", meta);
    res.send(newUrl.href);
  } catch (error) {
    return res.status(404).send(error);
  }
});
// Start server
app.listen(port, () => {
  console.log(`Example app listening on port ${port}`);
});
