# Running Rust code into an SGX enclave

## Install the SGX driver, SDK, and PSW

1. Run:
```bash
wget https://download.01.org/intel-sgx/linux-2.1.3/ubuntu64-desktop/sgx_linux_x64_driver_1bf506e.bin
wget https://download.01.org/intel-sgx/linux-2.1.3/ubuntu64-desktop/sgx_linux_x64_psw_2.1.103.44322.bin
wget https://download.01.org/intel-sgx/linux-2.1.3/ubuntu64-desktop/sgx_linux_x64_sdk_2.1.103.44322.bin
```

2. Clone the `rust-sgx-sdk` repo:
```bash
git clone https://github.com/baidu/rust-sgx-sdk.git
```

**Note:** The following instructions have been taken from the `rust-sgx-sdk` repo.

## Docker - Dev

1. Run:
```bash
# Should be done only once, to pull the image from dockerhub
docker pull baiduxlab/sgx-rust
docker run -v path/to/the/rust-sgx-sdk:/root/sgx -ti --device /dev/isgx baiduxlab/sgx-rust
```
2. In the container, run:
```bash
rustup default nightly-2018-03-16
rustup component add rust-src
```
3. Finally, test to run the hello-rust code sample:
```bash
# Start the aesm service in the background
# The Application Enclave Service Manager (AESM) is part of the Intel SGX
/opt/intel/sgxpsw/aesm/aesm_service &
```
4. Get the congtainer ID:
```bash
docker ps
```
5. Copy the enclave code of the repo into the docker container to compile and execute the application:
```bash
docker cp [ID-of-Your-Container] . /root/sgx/samplecode/rust-enclave
```
6. Run the `rust-enclave`:
```bash
cd sgx/samplecode/rust-enclave
make
cd bin
./app
```

**Note:** It might be necessary to run `SGX_MODE=SIM` before `make`.
