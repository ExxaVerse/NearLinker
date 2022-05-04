const express = require("express");
const nearAPI = require("near-api-js");
const https = require("https");
const fs = require("fs");
const { connect, keyStores, KeyPair, transactions, providers, utils } = nearAPI;
const crypto = require("crypto");

// Receive server configuration
const config = require("./config/config")();
const port = config.port;

// Initialize Express
const app = express();

// Initialize middlewares
app.use(express.json());

app.use(async (req, res, next) => {
  try {
    const account_id = req.body.account_id;
    const private_key = req.body.private_key;

    const keyStore = new keyStores.InMemoryKeyStore();
    if (account_id && private_key) {
      const decipher = crypto.createDecipheriv(
        config.encryption_algorithm,
        config.encryption_key,
        config.initialization_vector
      );

      const decrpyted_private_key = Buffer.concat([
        decipher.update(private_key, "base64"),
        decipher.final(),
      ]).toString();

      const keyPair = KeyPair.fromString(
        decrpyted_private_key.toString("ascii")
      );

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
  } catch (error) {
    next(error);
  }
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

  try {
    const wallet_id = req.params.wallet_id;

    await near
      .account(wallet_id)
      .then(async (account) => {
        return res.status(200).send(await account.getAccountBalance());
      })
      .catch((error) => res.status(404).send(error));
  } catch (error) {
    res.status(500).send(error);
  }
});

// VIEW - Call View contract function
app.get("/contract/:contract_id/:function_name", async (req, res) => {
  const near = req.near;
  try {
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
  } catch (error) {
    res.status(500).send(error);
  }
});

// CALL - Call contract function
app.post("/contract/:contract_id/call", async (req, res) => {
  const near = req.near;

  try {
    const account_id = req.body.account_id;
    const contract_id = req.params.contract_id;
    const function_name = req.body.function.name;
    const params = req.body.function.parameters || {};
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
  } catch (error) {
    res.status(500).send(error);
  }
});

// GET - Get transaction info
app.get(
  "/contract/:contract_id/transactions/:transaction_id",
  async (req, res) => {
    const near = req.near;

    try {
      const contract_id = req.params.contract_id;
      const transaction_id = req.params.transaction_id;

      const result = await near.connection.provider
        .txStatus(transaction_id, contract_id)
        .catch((error) => res.status(500).send(error));

      res.send(result);
    } catch (error) {
      res.status(500).send(error);
    }
  }
);

// TODO: GET - Get recent transactions for account
app.get(
  "/contract/:contract_id/transactions/:transaction_id",
  async (req, res) => {
    const near = req.near;
    try {
      const contract_id = req.params.contract_id;
      const transaction_id = req.params.transaction_id;

      const result = await near.connection.provider.txStatus(
        transaction_id,
        contract_id
      );

      res.send(result);
    } catch (error) {
      res.status(500).send(error);
    }
  }
);

// GET - Key pair
app.get("/keypair", async (_, res) => {
  try {
    const keypair = utils.KeyPair.fromRandom("ed25519");
    const cipher = crypto.createCipheriv(
      config.encryption_algorithm,
      config.encryption_key,
      config.initialization_vector
    );

    const encrypted_private_key = Buffer.concat([
      cipher.update(Buffer.from(keypair.secretKey.toString())),
      cipher.final(),
    ]).toString("base64");

    result = {
      public_key: keypair.publicKey.toString(),
      private_key: encrypted_private_key,
    };

    // Commented Last-Modified header since it could throw errors
    res
      .set("Last-Modified", new Date().toUTCString())
      .set("Expires", new Date().toUTCString())
      .set("Cache-Control", "no-cache")
      .set("Pragma", "no-cache")
      .send(result);
  } catch (error) {
    console.log(error);
    res.status(500).send(error);
  }
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
            utils.PublicKey.fromString(params.public_key),
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
    console.log(error);
    return res.status(404).send(error);
  }
});

// INIT SSL IF ENABLED
if (config.enable_ssl) {
  const options = {
    key: fs.readFileSync(config.ssl_key_path),
    cert: fs.readFileSync(config.ssl_cert_path),
	ca: [fs.readFileSync(config.ssl_ca_cert_path)],
  };

  // Start HTTPS server
  https.createServer(options, app).listen(port, () => {
    console.log(`HTTPS server started on port: ${port}`);
  });
} else {
  // Start HTTP server
  app.listen(port, () => {
    console.log(`HTTP server started on port: ${port}`);
  });
}
