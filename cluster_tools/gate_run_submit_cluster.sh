#!/usr/bin/env bash 

set -u
SCRIPTNAME="$(basename "${0}")"

function error {
echo "ERROR: $1"
usage
exit 1
}

DEFAULTRELEASESUFFIX="NONE"
DEFAULTNUMBEROFJOBS="10"

function usage {
echo "${SCRIPTNAME} mac/main.mac njobs releasesuffix paramtogate"
echo "default njobs = ${DEFAULTNUMBEROFJOBS}"
echo "default releasesuffix = ${DEFAULTRELEASESUFFIX} (NONE means use Gate in PATH)"
}

test $# -eq 0 && usage && exit 0

SCRIPTDIR="${HOME}/git/gate-tests/bin"
RELEASESUFFIX=${3:-"${DEFAULTRELEASESUFFIX}"}
RELEASEDIR="${HOME}/releases/grid_release${RELEASESUFFIX}"
JOBFILE="$(dirname $0)/gate_job_cluster.job"

echo "Checking stuff"
test -f ${JOBFILE} || error "can't find job file ${JOBFILE}"
if test "${RELEASESUFFIX}" = "${DEFAULTRELEASESUFFIX}"
then
	RELEASEDIR="NONE"
	which Gate 2>&1 >/dev/null || error "there is no Gate in the PATH"
else
	test -d ${RELEASEDIR} || error "invalid release dir ${RELEASEDIR}"
fi
MACRODIR=$(pwd)
test -d ${MACRODIR}/mac && test -d ${MACRODIR}/data || error "invalid path"
MACROFILE=${1:?"provide relative macro path"}
test -f ${MACRODIR}/${MACROFILE} || error "invalid macro"
OUTPUTDIR=$(mktemp --tmpdir=${MACRODIR} -d run.XXXX || error "can't create temp dir")
test -d ${OUTPUTDIR} || error "can't locate output dir"
RUNID=${OUTPUTDIR##*.}
NJOBS=${2:-"${DEFAULTNUMBEROFJOBS}"}
NJOBSMAX=${NJOBS}
PARAM="${4:-""}"

echo "Lets roll!!"
echo "runid is ${RUNID}"
echo "qsub is $(which qsub)"
test -z "${PARAM}" && echo "no param" || echo "param is ${PARAM}"
if test "$RELEASESUFFIX" = "$DEFAULTRELEASESUFFIX"
then
	echo "using $(which Gate)"
else
	echo "using release $(basename ${RELEASEDIR})"
fi
echo "submitting ${NJOBS} jobs"

PARAMFILE="${OUTPUTDIR}/params.txt"
echo "njobs = ${NJOBS}" >> "${PARAMFILE}"
echo "macro = ${MACROFILE}" >> "${PARAMFILE}"
test -z "${PARAM}" || echo "param = ${PARAM}" >> "${PARAMFILE}"

while test $NJOBS -gt 0; do
	qsub -N "gatejob.${RUNID}" -o "${OUTPUTDIR}" \
	-v "PARAM=\"${PARAM}\",INDEX=${NJOBS},INDEXMAX=${NJOBSMAX},SCRIPTDIR=${SCRIPTDIR},OUTPUTDIR=${OUTPUTDIR},RELEASEDIR=${RELEASEDIR},MACROFILE=${MACROFILE},MACRODIR=${MACRODIR}" \
	"${JOBFILE}" || error "submission error"
	let NJOBS--
done

echo "runid is ${RUNID}"