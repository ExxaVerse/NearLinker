const crypto = require("crypto");

module.exports = () => {
  const process_env = process.env.NODE_ENV || "development";
  const contract_name = process.env.CONTRACT_NAME || "lb4.testnet";
  const port = process.env.PORT || "3000";

  // TODO: read from file/as env variable
  const encryption_key =
    process.env.AES_KEY || "JaNcRfUjXn2r5u8x/A?D(G+KbPeSgVkY";
  const encryption_algorithm = process.env.AES_ALGORITHM || "aes-256-cbc";
  const initialization_vector = process.env.IV || crypto.randomBytes(16);
  let near_config;

  switch (process_env) {
    case "production":
    case "mainnet":
      near_config = {
        networkId: "mainnet",
        nodeUrl: "https://rpc.mainnet.near.org",
        walletUrl: "https://wallet.near.org",
        helperUrl: "https://helper.mainnet.near.org",
        explorerUrl: "https://explorer.mainnet.near.org",
      };
      break;
    case "development":
    case "testnet":
    default:
      near_config = {
        networkId: "testnet",
        nodeUrl: "https://rpc.testnet.near.org",
        walletUrl: "https://wallet.testnet.near.org",
        helperUrl: "https://helper.testnet.near.org",
        explorerUrl: "https://explorer.testnet.near.org",
      };
      break;
  }

  return {
    port: port,
    near_config: near_config,
    contract_name: contract_name,
    encryption_algorithm: encryption_algorithm,
    encryption_key: encryption_key,
    initialization_vector: initialization_vector,
  };
};
