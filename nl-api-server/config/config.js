module.exports = () => {
  const process_env = process.env.NODE_ENV || "development";
  const contract_name = process.env.CONTRACT_NAME || "lb4.testnet";
  const port = process.env.PORT || "3000";

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
  };
};
