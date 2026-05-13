FROM eclipse-temurin:17-jdk AS build
WORKDIR /workspace
COPY gradlew settings.gradle build.gradle ./
COPY gradle ./gradle
COPY carshow-events ./carshow-events
COPY carshow-grpc-contract ./carshow-grpc-contract
COPY storage-service ./storage-service
RUN chmod +x ./gradlew && ./gradlew :storage-service:bootJar --no-daemon -x test

FROM eclipse-temurin:17-jre
WORKDIR /app
COPY --from=build /workspace/storage-service/build/libs/*.jar /app/app.jar
EXPOSE 8082 9090
ENTRYPOINT ["java", "-jar", "/app/app.jar"]
