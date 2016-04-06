locate() {
    local SOURCE="${BASH_SOURCE[0]}"
    while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
        local DIR="$(cd -P "$( dirname "$SOURCE" )" && pwd)"
        SOURCE="$(readlink "$SOURCE")"
        # If $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
        [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
    done
    DIR="$(cd -P "$( dirname "$SOURCE" )/.." && pwd)"
    echo ${DIR}
}
