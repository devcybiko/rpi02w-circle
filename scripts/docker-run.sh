docker run -it \
  --name circle-dev \
  --platform linux/amd64 \
  -v "$PWD:/work" \
  -w /work \
  circle-dev \
  bash