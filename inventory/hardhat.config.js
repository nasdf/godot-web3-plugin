require('@nomicfoundation/hardhat-toolbox');
require('dotenv').config();

const POLYGON_API_KEY = process.env.POLYGON_API_KEY ?? '';
const MUMBAI_API_KEY = process.env.MUMBAI_API_KEY ?? '';

const accounts = process.env.PRIVATE_KEY ? [process.env.PRIVATE_KEY] : [];

/** @type import('hardhat/config').HardhatUserConfig */
module.exports = {
  solidity: '0.8.17',
   networks: {
    mumbai: {
      url: `https://polygon-mumbai.g.alchemy.com/v2/${MUMBAI_API_KEY}`,
      accounts,
    },
    polygon: {
      url: `https://polygon-mainnet.g.alchemy.com/v2/${POLYGON_API_KEY}`,
      accounts,
    },
  },
};
